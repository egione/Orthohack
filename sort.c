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
#define sort_free(sort_string_base) string_free((u8 *)(sort_string_base))

u64 *
sort_init(void){
  return (u64 *)(string_create(U8_SPAN<<U64_SIZE_LOG2));
}

void
sort_u64_list_sort(u64 *sort_string_base,u64 item_idx_max,u64 **item_list0_base_base,u64 **item_list1_base_base){
/*
Perform an early-out radix sort on a list of (u64)s.

In:

  item_idx_max is the maximum item number.

  **item_list0_base_base contains a list of (item_idx_max+1) (u64)s to sort.

  **item_list1_base_base contains as much space as **item_list0_base_base. Used for double buffering. Cannot overlap **item_list0_base_base.

Out:

    *item_list0_base_base may have been exchanged with *item_list1_base_base, but is otherwise unmodified. **item_list0_base_base contains the ascending sorted list.

    *item_list1_base_base may have been exchanged with *item_list0_base_base, but is otherwise unmodified. **item_list1_base_base is undefined.
*/
  u8 delta_shift;
  i8 delta_shift_max;
  u64 i;
  u64 *item_list0_base;
  u64 *item_list0_ptr;
  u64 *item_list1_base;
  u64 item_list_idx;
  u64 item_orsum;
  u64 item_prv;
  u8 item_u8;
  u64 *sort_string_ptr;
  u64 sum_nxt;
  u64 sum_prv;

  item_list0_base=*item_list0_base_base;
  item_list1_base=*item_list1_base_base;
  delta_shift_max=U64_BYTE_MAX<<U8_BITS_LOG2;
  item_orsum=0;
/*
Do a radix sort, in base U8_SPAN, over the bytes in each u64 at item_list0_base. Generally, this is bytes 0 through 7 of each item, but we might be able to cut that short below (see item_orsum).
*/
  for(delta_shift=0;delta_shift<=delta_shift_max;delta_shift+=U8_BITS){
    string_zero(U8_SPAN<<U64_SIZE_LOG2,(u8 *)(sort_string_base));
/*
While tallying u8 population counts for this byte column, move backwards to maximize cache hits because at the bottom of this loop, we move forwards.
*/
    item_list0_ptr=item_list0_base+item_idx_max;
    i=item_idx_max;
    do{
      item_u8=*item_list0_ptr>>delta_shift;
      item_list0_ptr--;
      sort_string_ptr=sort_string_base+item_u8;
      (*sort_string_ptr)++;
    }while(i--);
    sort_string_ptr=sort_string_base;
    sum_prv=0;
    sum_nxt=0;
/*
Compute the sorted u64 offset of each equivalence class (byte group) in this delta_shift.
*/
    for(i=0;i<=U8_MAX;i++){
      sum_nxt+=*sort_string_ptr;
      *sort_string_ptr=sum_prv;
      sort_string_ptr++;
      sum_prv=sum_nxt;
    }
/*
Sort all the items by the byte column implied by delta_shift.
*/
    item_list0_ptr=item_list0_base;
    i=item_idx_max;
    if(item_orsum==0){
/*
We can only get here once, even if the OR-sum of all the items, as computed below, turns out to be 0, in which case we'll set delta_shift_max=0 and never return here. Otherwise, we will hopefully be able to reduce delta_max and thereby exit sooner.
*/
      do{
        item_prv=*item_list0_ptr;
        item_list0_ptr++;
        sort_string_ptr=sort_string_base+(u8)(item_prv>>delta_shift);
        item_list_idx=*sort_string_ptr;
        item_list1_base[item_list_idx]=item_prv;
/*
While waiting for the bus, compute the OR of all items. If it turns out to be small enough, then we can reduce delta_shift_max and exit sooner.
*/
        item_orsum|=item_prv;
        *sort_string_ptr=item_list_idx+1;
      }while(i--);
      delta_shift_max=-U8_BITS;
      do{
        item_orsum>>=U8_BITS;
        delta_shift_max+=U8_BITS;
      }while(item_orsum);
/*
Increment item_orsum so that we won't waste time repeating the loop to find the most significant byte.
*/
      item_orsum++;
    }else{
      do{
        item_prv=*item_list0_ptr;
        item_list0_ptr++;
        sort_string_ptr=sort_string_base+(u8)(item_prv>>delta_shift);
        item_list_idx=*sort_string_ptr;
        item_list1_base[item_list_idx]=item_prv;
        *sort_string_ptr=item_list_idx+1;
      }while(i--);
    }
    item_list0_ptr=item_list0_base;
    item_list0_base=item_list1_base;
    item_list1_base=item_list0_ptr;
  }
  *item_list0_base_base=item_list0_base;
  *item_list1_base_base=item_list1_base;
  return;
}

void
sort_tag_list_sort(u64 *sort_string_base,u64 struct_size,u8 *struct_list_in_base,u8 *struct_list_out_base,u64 tag_idx_max,sort_tag_t **tag_list0_base_base,sort_tag_t **tag_list1_base_base){
/*
Sort a list of structs according a list of u64 tags.

In:

  sort_string_base is the return value of sort_init().

  struct_size is the size of each struct at struct_list_in_base, each of which pointed to by sort_tag_t.struct_base. It may be useful to set this to 0, in order to obtain a sorted list of bases, without paying the expense of actual memory relocation.

  *struct_list_in_base contains (tag_idx_max+1) items, each of size struct_size.

  *struct_list_out_base is writable for (tag_idx_max+1)*struct_size.

  tag_idx_max is the maximum sort_tag_t index.

  **tag_list0_base_base contains a list of (tag_idx_max+1) (sort_tag_t)s to sort based on their (item)s.

  **tag_list1_base_base contains as much space as **tag_list0_base_base. Used for double buffering. Cannot overlap **tag_list0_base_base.

Out:

  *tag_list0_base_base may have been exchanged with *tag_list1_base_base, but is otherwise unmodified. **tag_list0_base_base is equivalent to the input version, sorted ascending by item. (sort_tag_t)s with equal (item)s will appear in their original order.

  *tag_list1_base_base may have been exchanged with *tag_list0_base_base, but is otherwise unmodified. **tag_list1_base_base is undefined.

  *struct_list_out_base is the ascending sorted concatenation of all the struct_size structs pointed to by the input (**tag_list0_base_base) (struct_idx)s.
*/
  u8 delta_shift;
  i8 delta_shift_max;
  u64 i;
  u64 item_orsum;
  u8 item_u8;
  sort_tag_t tag_prv;
  u64 *sort_string_ptr;
  u64 sum_nxt;
  u64 sum_prv;
  u64 tag_list_idx;
  sort_tag_t *tag_list0_base;
  sort_tag_t *tag_list1_base;
  sort_tag_t *tag_list0_ptr;

  tag_list0_base=*tag_list0_base_base;
  tag_list1_base=*tag_list1_base_base;
  delta_shift_max=U64_BYTE_MAX*U8_BITS;
  item_orsum=0;
/*
Do a radix sort, in base U8_SPAN, over the bytes in each sort_tag_t at tag_list0_base. Generally, this is bytes 0 through 7 of each item, but we might be able to cut that short below (see item_orsum).
*/
  for(delta_shift=0;delta_shift<=delta_shift_max;delta_shift+=U8_BITS){
    string_zero(U8_SPAN<<U64_SIZE_LOG2,(u8 *)(sort_string_base));
/*
While tallying u8 population counts for this byte column, move backwards to maximize cache hits because at the bottom of this loop, we move forwards.
*/
    tag_list0_ptr=tag_list0_base+tag_idx_max;
    i=tag_idx_max;
    do{
      item_u8=(*tag_list0_ptr).item>>delta_shift;
      tag_list0_ptr--;
      sort_string_ptr=sort_string_base+item_u8;
      (*sort_string_ptr)++;
    }while(i--);
/*
If what we just incremented wrapped to 0, then all the u8's for this delta_shift are identical, i.e. we're done with this delta_shift. Otherwise, sort on it.
*/
    if(*sort_string_ptr){
      sort_string_ptr=sort_string_base;
      sum_prv=0;
      sum_nxt=0;
/*
Compute the sorted u64 offset of each equivalence class (byte group) in this delta_shift.
*/
      for(i=0;i<=U8_MAX;i++){
        sum_nxt+=*sort_string_ptr;
        *sort_string_ptr=sum_prv;
        sort_string_ptr++;
        sum_prv=sum_nxt;
      }
/*
Permute all the items around until they're in order for this delta_shift.
*/
      tag_list0_ptr=tag_list0_base;
      i=tag_idx_max;
      if(item_orsum==0){
/*
We can only get here once, even if the OR-sum of all the items, as computed below, turns out to be 0, in which case we'll set delta_shift_max=0 and never return here. Otherwise, we will hopefully be able to reduce delta_max and thereby exit sooner.
*/
        do{
          tag_prv=*tag_list0_ptr;
          tag_list0_ptr++;
          sort_string_ptr=sort_string_base+(u8)(tag_prv.item>>delta_shift);
          tag_list_idx=*sort_string_ptr;
          tag_list1_base[tag_list_idx]=tag_prv;
/*
While waiting for the bus, compute the OR of all items. If it turns out to be small enough, then we can reduce delta_shift_max and exit sooner.
*/
          item_orsum|=tag_prv.item;
          *sort_string_ptr=tag_list_idx+1;
        }while(i--);
        delta_shift_max=-U8_BITS;
        do{
          item_orsum>>=U8_BITS;
          delta_shift_max+=U8_BITS;
        }while(item_orsum);
/*
Increment item_orsum so that we won't waste time repeating the loop to find the most significant byte.
*/
        item_orsum++;
      }else{
        do{
          tag_prv=*tag_list0_ptr;
          tag_list0_ptr++;
          sort_string_ptr=sort_string_base+(u8)(tag_prv.item>>delta_shift);
          tag_list_idx=*sort_string_ptr;
          tag_list1_base[tag_list_idx]=tag_prv;
          *sort_string_ptr=tag_list_idx+1;
        }while(i--);
      }
      tag_list0_ptr=tag_list0_base;
      tag_list0_base=tag_list1_base;
      tag_list1_base=tag_list0_ptr;
    }
  }
  *tag_list0_base_base=tag_list0_base;
  *tag_list1_base_base=tag_list1_base;
  if(struct_size){
    do{
      memcpy((void *)(struct_list_out_base),(const void *)(struct_list_in_base+(tag_list0_base->struct_idx*struct_size)),struct_size);
      tag_list0_base++;
      struct_list_out_base+=struct_size;
    }while(tag_idx_max--);
  }
  return;
}

void
sort_struct_list_sort(u64 *idx_list_base,u8 *scratch_base,u64 *sort_string_base,u64 struct_idx_max,u8 *struct_list_base,u64 struct_size_minus_1){
/*
Perform a radix sort on a list of structs.

In:

  *idx_list_base is writable for size (((struct_idx_max+1)*U64_SIZE)<<1).

  *scratch_base is writable for size (struct_size_minus_1+1).

  struct_idx_max is the maximum struct number.

  *struct_list_base contains a list of (struct_idx_max+1) structs, each of size (struct_size_minus_1+1), to sort.

  struct_size_minus_1 is the size of each struct in the list, minus 1.

Out:

  *idx_list_base is undefined.

  *scratch_base is undefined.

  *struct_list_base contains the same structs, sorted ascending by the most significant byte.
*/
  u8 byte;
  u64 delta;
  u64 idx_idx_read_bias;
  u64 idx_idx_write_bias;
  u64 struct_count;
  u64 struct_countdown;
  u64 struct_idx_idx0;
  u64 struct_idx0;
  u64 struct_idx_idx1;
  u64 struct_idx1;
  u64 struct_idx_idx2;
  u64 struct_size;
  u64 sum_nxt;
  u64 sum_prv;

  struct_count=struct_idx_max+1;
  struct_size=struct_size_minus_1+1;
  struct_idx_idx0=0;
  struct_idx0=0;
  do{
    idx_list_base[struct_idx_idx0]=struct_idx0;
    struct_idx_idx0++;
    struct_idx0+=struct_size;
  }while(struct_idx_idx0!=struct_count);
  idx_idx_write_bias=0;
  delta=0;
/*
Do a radix sort, in base U8_SPAN, over the bytes in each struct at struct_list0_base.
*/
  do{
    string_zero(U8_SPAN<<U64_SIZE_LOG2,(u8 *)(sort_string_base));
/*
While tallying u8 population counts for this byte column, move backwards to maximize cache hits because at the bottom of this loop, we move forwards.
*/
    struct_idx_idx0=struct_idx_max;
    do{
      struct_idx0=idx_list_base[struct_idx_idx0];
      byte=struct_list_base[struct_idx0+delta];
      sort_string_base[byte]++;
    }while(struct_idx_idx0--);
/*
Compute the sorted base index of each equivalence class (byte group) for this delta. We toggle idx_idx_write_bias between 0 and struct_count in order to double buffer.
*/
    struct_idx_idx0=idx_idx_write_bias;
    idx_idx_write_bias=struct_count-idx_idx_write_bias;
    sum_nxt=0;
    sum_prv=0;
    byte=0;
    do{
      sum_nxt+=sort_string_base[byte];
      sort_string_base[byte]=sum_prv+idx_idx_write_bias;
      sum_prv=sum_nxt;
    }while((byte++)!=U8_MAX);
/*
Sort all the structs by the byte column implied by delta.
*/
    for(struct_countdown=struct_count;struct_countdown!=0;struct_countdown--){
      struct_idx0=idx_list_base[struct_idx_idx0];
      struct_idx_idx0++;
      byte=struct_list_base[struct_idx0+delta];
      struct_idx_idx1=sort_string_base[byte];
      idx_list_base[struct_idx_idx1]=struct_idx0;
      struct_idx_idx1++;
      sort_string_base[byte]=struct_idx_idx1;
    }
    delta++;
  }while(delta<struct_size);
  idx_idx_read_bias=struct_count-idx_idx_write_bias;
  struct_idx_idx1=struct_idx_max;
  do{    
    struct_idx_idx0=idx_list_base[struct_idx_idx1+idx_idx_write_bias];
    struct_idx_idx0/=struct_size;
    idx_list_base[struct_idx_idx1+idx_idx_write_bias]=struct_idx_idx0;
    idx_list_base[struct_idx_idx0+idx_idx_read_bias]=struct_idx_idx1;
  }while(struct_idx_idx1--);
  struct_idx_idx1=0;
  do{
    struct_idx1=idx_list_base[struct_idx_idx1+idx_idx_write_bias];
    struct_idx0=idx_list_base[struct_idx_idx1+idx_idx_read_bias];
    idx_list_base[struct_idx0+idx_idx_write_bias]=struct_idx1;
    idx_list_base[struct_idx1+idx_idx_read_bias]=struct_idx0;
    struct_idx0=struct_idx_idx1*struct_size;
    struct_idx1*=struct_size;
    memcpy((void *)(scratch_base),(const void *)(&struct_list_base[struct_idx0]),struct_size);   
    memcpy((void *)(&struct_list_base[struct_idx0]),(const void *)(&struct_list_base[struct_idx1]),struct_size);   
    memcpy((void *)(&struct_list_base[struct_idx1]),(const void *)(scratch_base),struct_size);
    struct_idx_idx1++;
  }while(struct_idx_idx1!=struct_count);
  return;
}
