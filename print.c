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
const u8 *error_string_base="\nERROR: ";
u8 *intro_string_base="http://orthohack.blogspot.com\nCopyright 2016 Russell Leidich\n\n";
/*
These are the names of the precision modes. Append as needed.
*/
u8 *precision0_string_base="32-bit single";
u8 *precision1_string_base="64-bit double";
u8 *precision2_string_base="80-padded-to-96-bit long double";

u8 *precision_unknown_error_string_base="print_precision() is not aware of the requested numerical precision";

void
print_error(u8 *string_base){
  printf("%s%s.\n",(const char *)(error_string_base),(const char *)(string_base));
  return;
}

void
print_error_exit(u8 *string_base){
  print_error(string_base);
  exit(1);
}

void
print_error_exit_out_of_memory(void){
  print_error_exit("Out of memory");
}

void
print_error_value_exit(u8 *string_base,u32 value){
  printf("%s",(const char *)(error_string_base));
  printf(string_base,value);
  printf(".\n");
  exit(1);
}

void
print_error_string_exit(u8 *string0_base,u8 *string1_base){
  printf("%s%s%s.\n",(const char *)(error_string_base),(const char *)(string0_base),(const char *)(string1_base));
  exit(1);
}

void
print_new_line(void){
  printf("\n");
}

void
print_u64(u64 value){
  printf("0x%08X%08X",(u32)(value>>U32_BITS),(u32)(value));
  fflush(stdout);
  return;
}

u8
print_string_as_u64(u8 column_idx,u64 *u64_list_base,u64 u64_count,u8 i64_not_u64){
  u64 idx;
  u64 value;

  idx=0;
  while(idx<u64_count){
    value=u64_list_base[idx];
    printf("0x%08X%08X",(u32)(value>>U32_BITS),(u32)(value));
    if(!i64_not_u64){
      printf("U");
    }
    printf("LL");
    idx++;
    if(idx!=u64_count){
      printf(",");
      column_idx++;
      if(column_idx==3){
        column_idx=0;
      }
      if(!column_idx){
        printf(" \\\n");
      }
    }
  }
  return column_idx;
}

u8
print_u64_string_end(u8 column_idx,u64 idx,u64 idx_max){
  column_idx++;
  if(column_idx==3){
    column_idx=0;
  }
  if(idx!=idx_max){
    printf(",");
    if(!column_idx){
      printf(" \\\n");
    }
  }else{
    printf("\n");
  }
  return column_idx;
}

u8
print_string_as_i32(u8 column_idx,i32 *i32_list_base,u64 i32_count){
  u64 idx;

  idx=0;
  while(idx<i32_count){
    printf("0x%08XL",i32_list_base[idx]);
    idx++;
    if(idx!=i32_count){
      printf(",");
      column_idx++;
      if(column_idx==5){
        column_idx=0;
      }
      if(!column_idx){
        printf(" \\\n");
      }
    }
  }
  return column_idx;
}

u8
print_i32_string_end(u8 column_idx,u64 idx,u64 idx_max){
  column_idx++;
  if(column_idx==5){
    column_idx=0;
  }
  if(idx!=idx_max){
    printf(",");
    if(!column_idx){
      printf(" \\\n");
    }
  }else{
    printf("\n");
  }
  return column_idx;
}

u8
print_string_as_u8(u8 column_idx,u8 *string_base,u64 string_size){
  u64 idx;

  idx=0;
  while(idx<string_size){
    printf("0x%02X",string_base[idx]);
    idx++;
    if(idx!=string_size){
      printf(",");
      column_idx++;
      column_idx&=7;
      if(!column_idx){
        printf(" \\\n");
      }
    }
  }
  return column_idx;
}

u8
print_u8_string_end(u8 column_idx,u64 idx,u64 idx_max){
  column_idx++;
  column_idx&=7;
  if(idx!=idx_max){
    printf(",");
    if(!column_idx){
      printf(" \\\n");
    }
  }else{
    printf("\n");
  }
  return column_idx;
}

void
print_u64_as_u8(u64 value){
  printf("0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",(u8)(value),(u8)(value>>8),(u8)(value>>16),(u8)(value>>24),(u8)(value>>32),(u8)(value>>40),(u8)(value>>48),(u8)(value>>56));
  fflush(stdout);
  return;
}

void
print_define(u8 *string_base,u64 value){
  if(!(value>>U32_BITS)){
    printf("#define %s 0x%08XUL\n",string_base,(u32)(value));
  }else{
    printf("#define %s 0x%08X%08XULL\n",string_base,(u32)(value>>U32_BITS),(u32)(value));
  }
  return;
}

void
print_define_decimal(u8 *string_base,u32 value){
  printf("#define %s %d\n",string_base,value);
  return;
}

void
print_u8(u8 value){
  printf("0x%02X\n",value);
  fflush(stdout);
  return;
}

void
print(u8 *string_base){
  printf("%s",string_base);
  fflush(stdout);
  return;
}

void
print_precision(u8 precision,u8 input_or_internal_or_output){
  u8 *precision_string_base;

  printf("Using ");
  switch(precision){
  case 0:
    precision_string_base=precision0_string_base;
    break;
  case 1:
    precision_string_base=precision1_string_base;
    break;
  case 2:
    precision_string_base=precision2_string_base;
    break;
  default:
    print_error_exit(precision_unknown_error_string_base);
  }
  printf("%s precision for ",precision_string_base);
  switch(input_or_internal_or_output){
  case 0:
    printf("file input");
    break;
  case 1:
    printf("internal calculation");
    break;
  case 2:
    printf("file output");
  }
/*
We switch from printf() to print() in order to fflush(stdout).
*/
  print(".\n");
  return;
}

void
print_intro(u8 *subtitle_string_base){
  printf("Orthohack, build %d\n%s\n",ORTHOHACK_BUILD_ID,subtitle_string_base);
  print(intro_string_base);
  return;
}
