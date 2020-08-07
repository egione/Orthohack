/*
Orthohack
Copyright 2016 Russell Leidich
http://orthohack.blogspot.com

This collection of files constitutes the Orthohack Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Orthohack Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Orthohack Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Orthohack Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
#ifndef DEBUG
#define string_free(string_base) free(string_base)
#else
#define string_free(string_base) \
  free(string_base); \
  debug_string_count-=!!string_base
#endif

u8 *
string_create(u64 string_size){
/*
Create a string.

Out:

  Returns NULL if string_size==0, or there isn't enough memory, else the base of the new string.

  See guarantees provided by U64_EXTEND().
*/
  u8 *string_base;
  u64 string_size_extended;

  string_size_extended=U64_EXTEND(string_size);
  if(string_size_extended>U64_SIZE){
#ifdef _32_
    if((string_size_extended>>32)==0){
#endif
      string_base=(u8 *)(malloc((size_t)(string_size_extended)));
#ifdef _32_
    }else{
      string_base=NULL;
    }
#endif
#ifdef DEBUG
    debug_string_count+=!!string_base;
#endif
  }else{
    string_base=NULL;
  }
  return string_base;
}

void
string_fill(u64 string_size,u8 *string_base,u8 default_byte){
  if(string_size){
    memset((void *)(string_base),(int)(default_byte),(size_t)(string_size));
  }
  return;
}

void
string_zero(u64 string_size,u8 *string_base){
  string_fill(string_size,string_base,0);
  return;
}

u8 *
string_create_zero(u64 string_size){
/*
Create a zeroed string.

Out:

  Returns NULL if string_size==0, or there isn't enough memory, else the base of the new string.

  See guarantees provided by U64_EXTEND().
*/
  u8 *string_base;

  if(string_base=string_create(string_size)){
    string_zero(string_size,string_base);
  }
  return string_base;
}

u8
string_u64_product(u64 p,u64 q,u64 *p_times_q_base){
/*
Return the product of 2 (u64)s as a u64, if possible.

In:

  p is the first factor.

  q is the second factor.

Out:

  Returns 0 if the unsigned product of p and q is representable in a u64, else 1.

  *p_times_q_base is the low u64 of the 128-bit unsigned product of p and q.
*/
  u64 product_hi;
  u64 product_lo;
  u64 product_lo_prv;
  u64 product_mid;

  product_hi=(p>>U32_BITS)*(q>>U32_BITS);
  product_lo=(u64)((u32)(p))*(u32)(q);
  product_mid=(p>>U32_BITS)*(u32)(q);
  product_lo_prv=product_lo;
  product_lo+=product_mid<<U32_BITS;
  product_hi+=(product_mid>>U32_BITS)+(product_lo<product_lo_prv);
  product_mid=(u32)(p)*(q>>U32_BITS);
  product_lo_prv=product_lo;
  product_lo+=product_mid<<U32_BITS;
  product_hi+=(product_mid>>U32_BITS)+(product_lo<product_lo_prv);
  *p_times_q_base=product_lo;
  return !!product_hi;
}

u8 *
string_bitmap_create(u64 field_count,u64 field_bit_count,u8 default_bit,u64 *size_base){
/*
In:

  field_count is the number of bitfields in the bitmap, > 0.

  field_bit_count is the number of bits per field, > 0.

  default_bit is the requested state of all bits, on [0,1].

  *size_base is undefined.
Out:

  Returns NULL on failure, else the base of the bitmap. The bitmap consists of field_count*field_bit_count bits, rounded up to the nearest byte. Any overallocated bits in the last byte are 0 (regardless of default_bit) in order to prevent the leakage of private data. Bits in the U64_EXTEND() area are undefined, and merely present as a convenience for avoiding memory faults.

  See guarantees provided by U64_EXTEND().

  *size_base is the (byte) size of the hull for future calls to string_zero(). Take care when calling string_fill() with a nonzero value if there may be tail bits in the last byte, because privacy conventions suggest that such bits should always be set to 0 before emission to storage media.
*/
  u8 bit_tail_count;
  u8 *bitmap_base;
  u64 field_size;

  if((string_u64_product(field_count,field_bit_count,&field_bit_count)==0)&&field_bit_count){
    bit_tail_count=field_bit_count&U8_BIT_MAX;
    field_size=(field_bit_count>>U8_BITS_LOG2)+(!!bit_tail_count);
    bitmap_base=string_create(field_size);
    if(bitmap_base){
      default_bit=(-default_bit);
      string_fill(field_size,bitmap_base,default_bit);
/*
Fix the last byte of the bitmap so that any overallocated bits (excluding the undefined U64_EXTEND readable region) are 0, in order to prevent unauthorized info from leaking into the file.
*/
      bitmap_base[field_size-1]=default_bit>>((U8_BITS-bit_tail_count)&U8_BIT_MAX);
      *size_base=field_size;
    }
  }else{
    bitmap_base=NULL;
  }
  return bitmap_base;
}

u8 *
string_item_list_create(u64 item_count,u64 item_size){
/*
In:

  item_count is the number of items in the list.

  item_size is the size of an item.

Out:

  Returns NULL on failure, else the base of the list.

  See guarantees provided by U64_EXTEND().
*/
  u8 *item_list_base;

  if(string_u64_product(item_count,item_size,&item_size)==0){
    item_list_base=string_create(item_size);
  }else{
    item_list_base=NULL;
  }
  return item_list_base;
}

u8 *
string_item_list_create_zero(u64 item_count,u64 item_size){
/*

In:

  item_count is the number of items in the list.

  item_size is the size of an item.

Out:

  Returns NULL on failure, else the base of the list, which has been initialized to 0.

  See guarantees provided by U64_EXTEND().
*/
  u8 *item_list_base;

  if(string_u64_product(item_count,item_size,&item_size)==0){
    item_list_base=string_create_zero(item_size);
  }else{
    item_list_base=NULL;
  }
  return item_list_base;
}

void
string_list_free(u64 string_count,u8 **string_base_list_base){
/*
Free a set of strings, and the string containing a list of their bases.

Out:

  string_count strings with bases at *string_base_list_base are freed, as is the list itself.

  See guarantees provided by U64_EXTEND().
*/

  if(string_count&&string_base_list_base){
    do{
      string_count--;
      string_free(string_base_list_base[string_count]);
    }while(string_count);
  }
  string_free(string_base_list_base);
  return;
}

u8 **
string_list_create(u64 string_count,u64 string_size){
/*
Create a set of strings, then create a string consisting of their bases.

Out:

  Returns NULL if string_size==0, or there isn't enough memory, else the base of the new string.

  See guarantees provided by U64_EXTEND().
*/
  u64 i;
  u8 status;
  u8 *string_base;
  u8 **string_base_list_base;

  status=1;
  if(string_size&&string_count){
    string_base_list_base=(u8 **)(string_item_list_create_zero(string_count,PTR_SIZE));
    if(string_base_list_base){
      i=0;
      do{
        string_base=string_create(string_size);
        status=!string_base;
        if(status){
          break;
        }
        string_base_list_base[i]=string_base;
        i++;
      }while(i!=string_count);
      if(status){
        string_list_free(string_count,string_base_list_base);
      }
    }
  }
  if(status){
    string_base_list_base=NULL;
  }
  return string_base_list_base;
}

u8 **
string_list_create_zero(u64 string_count,u64 string_size){
/*
Create a set of zeroed strings, then create a string consisting of their bases.

Out:

  Returns NULL if string_size==0, or there isn't enough memory, else the base of the new string.

  See guarantees provided by U64_EXTEND().
*/
  u64 i;
  u8 status;
  u8 *string_base;
  u8 **string_base_list_base;

  status=1;
  if(string_size&&string_count){
    string_base_list_base=(u8 **)(string_item_list_create_zero(string_count,PTR_SIZE));
    if(string_base_list_base){
      i=0;
      do{
        string_base=string_create_zero(string_size);
        status=!string_base;
        if(status){
          break;
        }
        string_base_list_base[i]=string_base;
        i++;
      }while(i!=string_count);
      if(status){
        string_list_free(string_count,string_base_list_base);
      }
    }
  }
  if(status){
    string_base_list_base=NULL;
  }
  return string_base_list_base;
}

u8
string_realloc(u64 string_size_new,u64 string_size,u8 **string_base_base){
/*
In:

  string_size_new is the desired size of the reallocated string, which may be any size. If string_size==0, then string_size_new==0 will invoke string_free(*string_base_base). In any event, string_size_new==0 will return 0 with *string_base_base==NULL.

  string_size is the current size of the string.

  *string_base_base is the base of the string to reallocate.

Out:

  Returns 1 on failure, else 0. On failure, **string_base_base is left intact and *string_base_base remains allocated. Not guaranteed to succeed if (string_size_new!=0), even if (string_size_new<string_size).

  Other guarantees provided by U64_EXTEND().
*/
  u8 status;
  u8 *string_base;
  u64 string_size_new_extended;

  status=0;
  string_base=*string_base_base;
  string_size_new_extended=U64_EXTEND(string_size_new);
  if(string_size_new_extended>U64_SIZE){
    if(string_size){
#ifdef _32_
      if((string_size_new_extended>>U32_BITS)==0){
#endif
        string_base=(u8 *)(realloc((void *)(string_base),(size_t)(string_size_new_extended)));
        status=!string_base;
#ifdef _32_
      }else{
        status=1;
      }
#endif
    }else{
      string_base=string_create(string_size_new);
      status=!string_base;
    }
  }else{
    if(string_size_new){
      status=1;
    }else{
      if(string_size){
        string_free(string_base);
      }
      string_base=NULL;
    }
  }
  if(status==0){
    *string_base_base=string_base;
  }
  return status;
}

u8
string_item_list_realloc(u64 item_count_new,u64 item_count,u64 item_size,u8 **item_list_base_base){
/*
In:

  item_count_new is the desired number of items in the list.

  item_count is the number of items in the list.

  item_size is the size of an item.

Out:

  Returns 1 on failure, else 0.

  *item_list_base_base is the base of the reallocated list.

  See guarantees provided by U64_EXTEND().
*/
  u8 *item_list_base;
  u64 item_list_size;
  u64 item_list_size_new;
  u8 status;

  status=string_u64_product(item_count,item_size,&item_list_size);
  status|=string_u64_product(item_count_new,item_size,&item_list_size_new);
  if(!status){
    item_size*=item_count;
    status=string_realloc(item_list_size_new,item_list_size,item_list_base_base);
  }
  return status;
}

