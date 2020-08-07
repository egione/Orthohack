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
#ifdef DEBUG
static u32 debug_context=0;
static u32 debug_exit_counter=0;
static u32 debug_string_count=0;
static u8 debug_print_status=1;
static u8 debug_print_flush_status=1;

void
debug_print_off(void){
  debug_print_status=0;
  return;
}

void
debug_print_on_context(u32 context){
  if(debug_context==context){
    debug_print_status=1;
  }
  debug_context++;
  return;
}

void
debug_print_on(void){
  debug_print_status=1;
  return;
}

void
debug_print_flush(void){
  if(debug_print_flush_status){
    fflush(stdout);
  }
  return;
}

void
debug_print_flush_off(void){
  debug_print_flush_status=0;
  return;
}

void
debug_print_flush_on(void){
  debug_print_flush_status=1;
  return;
}

void
debug_print(u8 *string_base){
  if(debug_print_status){
    printf("%s",string_base);
    debug_print_flush();
  }
  return;
}

void
debug_string_count_print(void){
  if(debug_print_status){
    printf("debug_string_count=%08X\n",debug_string_count);
    debug_print_flush();
  }
  return;
}

u8
debug_context_compare(u32 context){
  return debug_context==context;
}

u8
debug_context_at_least(u32 context){
  return debug_context>=context;
}

void
debug_context_print(){
  if(debug_print_status){
    printf("debug_context=%08X\n",debug_context);
    debug_print_flush();
  }
  return;
}

void
debug_context_inc(void){
  debug_context++;
  return;
}

void
debug_context_print_inc(void){
  if(debug_print_status){
    printf("%08X",debug_context);
    debug_print_flush();
  }
  debug_context++;
  return;
}

void
debug_context_print_inc_sparse(u32 context_base,u8 period_log2){
  if(debug_context>=context_base){
    if(((debug_context>>period_log2)<<period_log2)==debug_context){
      printf("debug_context=%08X\n",debug_context);
      debug_print_flush();
    }
  }
  debug_context++;
  return;
}

void
debug_exit(u32 context){
  if(debug_exit_counter==context){
    exit(1);
  }
  debug_exit_counter++;
  return;
}

void
debug_fatal(u8 *string_base){
  printf("ERROR: %s\n",string_base);
  exit(1);
}

u8
debug_fail(u32 context){
  u8 status;

  status=(debug_context==context);
  if(status){
    if(debug_print_status){
      printf("DEBUG SIMULATED FAILURE.\n");
      debug_print_flush();
    }
  }
  debug_context++;
  return status;
}

void
debug_bitmap_print(const char *context_string_base,u8 *bitmap_base,u64 bit_idx,u32 bit_count){
  u32 chunk_count;
  u8 column_idx;

  if(debug_print_status){
    printf("%s[%08X]={\n",context_string_base,bit_count);
    column_idx=0;
    chunk_count=bit_count>>U64_BITS_LOG2;
    while(chunk_count--){
      printf("  ");
      do{
        if(BIT_TEST(bitmap_base,bit_idx)){
          printf("1");
        }else{
          printf("0");
        }
        bit_idx++;
        column_idx++;
      }while(column_idx!=U64_BITS);
      printf("\n");
      column_idx=0;
    }
    bit_count&=U64_BIT_MAX;
    if(bit_count){
      printf("  ");
      do{
        if(BIT_TEST(bitmap_base,bit_idx)){
          printf("1");
        }else{
          printf("0");
        }
        bit_idx++;
      }while(--bit_count);
      printf("\n");
    }
    printf("}\n");
  }
  if(debug_print_status){
    debug_print_flush();
  }
  return;
}

void
debug_string_print_custom(const char *context_string_base,u32 chunk_count,u8 *string_base,u8 chunk_size_log2,u8 backwards,u8 compilable){
  u8 char_array_base[2+(8*11)+4];
  u8 char_array_idx;
  u8 char_array_idx_delta;
  u8 column_count;
  u32 i;
  u8 j;
  u32 k;
  i8 k_delta;
  u32 row_count;

  if(chunk_size_log2<=U64_SIZE_LOG2){
    if(debug_print_status){
      if(compilable==0){
        printf("%s[%08X]={",context_string_base,chunk_count);
      }else{
        printf("%s \\",context_string_base);
      }
      printf("\n");
    }
    if(string_base){
      row_count=4;
      column_count=0x10;
      if(chunk_size_log2>1){
        row_count++;
        column_count<<=1;
      }
      row_count-=chunk_size_log2;
      row_count=(chunk_count>>row_count)+(!!(chunk_count&((1<<row_count)-1)));
      column_count>>=chunk_size_log2;
      k_delta=1<<chunk_size_log2;
      char_array_idx_delta=(k_delta<<1)+1;
      if(backwards==0){
        k=0;
      }else{
        k_delta=-k_delta;
        k=(chunk_count<<chunk_size_log2)+k_delta;
      }
      for(i=0;i<row_count;i++){
        if(i==(row_count-1)){
          column_count=chunk_count;
        }
        char_array_idx=0;
        for(j=0;j<column_count;j++){
          if(compilable){
            char_array_base[char_array_idx]='0';
            char_array_base[char_array_idx+1]='x';
            char_array_idx+=2;
          }
          switch(chunk_size_log2){
          case 0:
            sprintf((char *)(char_array_base+char_array_idx),"%02X,",string_base[k]);
            break;
          case 1:
            sprintf((char *)(char_array_base+char_array_idx),"%04X,",*(u16 *)(string_base+k));
            break;
          case 2:
            sprintf((char *)(char_array_base+char_array_idx),"%08X,",*(u32 *)(string_base+k));
            break;
          case 3:
            sprintf((char *)(char_array_base+char_array_idx),"%08X%08X,",*(u32 *)(string_base+k+U32_SIZE),*(u32 *)(string_base+k));
            break;
          }
          char_array_idx+=char_array_idx_delta;
          k+=k_delta;
        }
        chunk_count-=column_count;
        if(i==(row_count-1)){
          char_array_idx--;
        }
        if(compilable&&(i!=(row_count-1))){
          char_array_base[char_array_idx]=' ';
          char_array_base[char_array_idx+1]='\\';
          char_array_idx+=2;
        }
        char_array_base[char_array_idx]='\n';
        char_array_base[char_array_idx+1]=0;
        if(debug_print_status){
          printf("  %s",(char *)(char_array_base));
        }
      }
      if(debug_print_status){
        if(compilable==0){
          printf("}");
        }
        printf("\n");
      }
    }else{
      printf("\ndebug_string_print_custom(): String has NULL base.\n");
      exit(1);
    }
  }else{
    printf("\ndebug_string_print_custom(): chunk_size_log2>%d.\n",U64_SIZE_LOG2);
    exit(1);
  }
  if(debug_print_status){
    debug_print_flush();
  }
  return;
}

void
debug_string_print(const char *context_string_base,u32 chunk_count,u8 *string_base,u8 chunk_size_log2){
  debug_string_print_custom(context_string_base,chunk_count,string_base,chunk_size_log2,0,0);
  return;
}

void
debug_float_double_list_print_custom(const char *context_string_base,u32 chunk_count,u8 *string_base,u8 float_or_double_or_long_double_status,u8 backwards,u8 compilable){
#define LINE_SIZE_MAX (2+(16*4)+4)
  u8 char_array_base[LINE_SIZE_MAX];
  u8 char_array_idx;
  u8 char_array_idx_delta;
  u8 column_count;
  u8 column_count_log2;
  u32 i;
  u8 j;
  u32 k;
  i8 k_delta;
  u8 nonnegative_status;
  u32 row_count;

  if(debug_print_status){
    if(compilable==0){
      printf("%s[%08X]={",context_string_base,chunk_count);
    }else{
      printf("%s \\",context_string_base);
    }
    printf("\n");
  }
  if(string_base){
    switch(float_or_double_or_long_double_status){
    case 0:
      k_delta=sizeof(float);
      break;
    case 1:
      k_delta=sizeof(double);
      break;
    case 2:
      k_delta=sizeof(long double);
    }
    if(float_or_double_or_long_double_status==0){
      column_count_log2=0;
      char_array_idx_delta=0x10;
    }else{
      column_count_log2=1;
      char_array_idx_delta=0x19;
    }
    column_count=1<<column_count_log2;
    row_count=(chunk_count>>column_count_log2)+(!!(chunk_count&(column_count-1)));
    if(backwards==0){
      k=0;
    }else{
      k_delta=-k_delta;
      k=(chunk_count<<(float_or_double_or_long_double_status+2))+k_delta;
    }
    for(i=0;i<row_count;i++){
      if(i==(row_count-1)){
        column_count=chunk_count;
      }
      char_array_idx=0;
      for(j=0;j<LINE_SIZE_MAX;j++){
        char_array_base[j]='\x00';
      }
      for(j=0;j<column_count;j++){
        nonnegative_status=0;
        switch(float_or_double_or_long_double_status){
        case 0:
          if(*(float *)(string_base+k)>=0.0f){
            nonnegative_status=1;
          }
          snprintf((char *)(char_array_base+char_array_idx+nonnegative_status),0x10,"%1.7E,",*(float *)(string_base+k));
          break;
        case 1:
          if(*(double *)(string_base+k)>=0.0f){
            nonnegative_status=1;
          }
          snprintf((char *)(char_array_base+char_array_idx+nonnegative_status),0x19,"%1.16E,",*(double *)(string_base+k));
          break;
        case 2:
          if(*(long double *)(string_base+k)>=0.0f){
            nonnegative_status=1;
          }
          snprintf((char *)(char_array_base+char_array_idx+nonnegative_status),0x19,"%1.16E,",(double)(*(long double *)(string_base+k)));
        }
        char_array_idx+=char_array_idx_delta;
        k+=k_delta;
      }
      for(j=0;j<LINE_SIZE_MAX;j++){
        if(char_array_base[j]=='\x00'){
          char_array_base[j]=' ';
        }
      }
      chunk_count-=column_count;
      if(i==(row_count-1)){
        char_array_idx--;
      }
      if(compilable&&(i!=(row_count-1))){
        char_array_base[char_array_idx]=' ';
        char_array_base[char_array_idx+1]='\\';
        char_array_idx+=2;
      }
      char_array_base[char_array_idx]='\n';
      char_array_base[char_array_idx+1]=0;
      if(debug_print_status){
        printf("  %s",(char *)(char_array_base));
      }
    }
    if(debug_print_status){
      if(compilable==0){
        printf("}");
      }
      printf("\n");
    }
  }else{
    printf("\ndebug_float_double_print_custom(): String has NULL base.\n");
    exit(1);
  }
  if(debug_print_status){
    debug_print_flush();
  }
  return;
}

void
debug_double_list_print(const char *context_string_base,u32 chunk_count,double *string_base){
  debug_float_double_list_print_custom(context_string_base,chunk_count,(u8 *)(string_base),1,0,0);
  return;
}

void
debug_float_list_print(const char *context_string_base,u32 chunk_count,float *string_base){
  debug_float_double_list_print_custom(context_string_base,chunk_count,(u8 *)(string_base),0,0,0);
  return;
}

void
debug_long_double_list_print(const char *context_string_base,u32 chunk_count,long double *string_base){
  debug_float_double_list_print_custom(context_string_base,chunk_count,(u8 *)(string_base),2,0,0);
  return;
}

void
debug_double(u8 *name_base,double value){
  if(debug_print_status){
    printf("%s=%1.16E\n",name_base,value);
    debug_print_flush();
  }
  return;
}

void
debug_float(u8 *name_base,float value){
  if(debug_print_status){
    printf("%s=%1.8E\n",name_base,value);
    debug_print_flush();
  }
  return;
}

void
debug_long_double(u8 *name_base,long double value){
  if(debug_print_status){
    printf("%s=%1.16E\n",name_base,(double)(value));
    debug_print_flush();
  }
  return;
}

void
debug_u8(u8 *name_base,u8 value){
  if(debug_print_status){
    printf("%s=%02X\n",name_base,value);
    debug_print_flush();
  }
  return;
}

void
debug_u16(u8 *name_base,u16 value){
  if(debug_print_status){
    printf("%s=%04X\n",name_base,value);
    debug_print_flush();
  }
  return;
}

void
debug_u32(u8 *name_base,u32 value){
  if(debug_print_status){
    printf("%s=%08X\n",name_base,value);
    debug_print_flush();
  }
  return;
}

void
debug_u64(u8 *name_base,u64 value){
  if(debug_print_status){
    printf("%s=%08X%08X\n",name_base,(u32)(value>>32),(u32)(value));
    debug_print_flush();
  }
  return;
}

void
debug_ptr(u8 *name_base,void *base){
#ifdef _64_
  debug_u64(name_base,(u64)(base));
#else
  debug_u32(name_base,(u32)(base));
#endif
  return;
}
#endif
