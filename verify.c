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
#include "unix_include.h"
#include "build.h"
#include "orthoconstant.h"
#include "config.h"
#undef COEFF_COUNT
#undef PRECISION_INTERNAL
#include "orthohack.h"
#include "debug.h"
#include "orthodim.h"
#include "orthonumber.h"
#include "orthofixed.h"
#include "orthocustom.h"
#include "orthofloat.c"
#include "orthofloat_x8.c"
#include "orthofixed.c"
#include "orthofixed_x8.c"
#include "orthocustom.c"
#include "orthocustom_x8.c"
#include "file_sys.h"
#undef PRECISION_INTERNAL
#include "verify.h"
#include "number_io.h"
#include "tree.h"
#include "debug.c"
#include "file_sys.c"
#include "string.c"
#include "print.c"

int
main(int argc, char *argv[]){
  NUMBER coeff_custom_error;
  NUMBER coeff_custom_error_max;
  NUMBER coeff_custom_error_mean_squared;
  CUSTOM coeff_custom_max;
  CUSTOM coeff_custom_min;
  CUSTOM *coeff_custom_x8_list_base;
  CUSTOM coeff_custom0;
  CUSTOM coeff_custom0_x8[8];
  CUSTOM coeff_custom1;
  NUMBER coeff_error;
  NUMBER coeff_fixed_error;
  NUMBER coeff_fixed_error_max;
  NUMBER coeff_fixed_error_mean_squared;
  FIXED coeff_fixed_max;
  FIXED coeff_fixed_min;
  FIXED *coeff_fixed_x8_list_base;
  FIXED coeff_fixed0;
  FIXED coeff_fixed0_x8[8];
  FIXED coeff_fixed1;
  NUMBER coeff_float_error_max;
  NUMBER coeff_float_error_mean_squared;
  NUMBER coeff_float_max;
  NUMBER coeff_float_min;
  NUMBER *coeff_float_x8_list_base;
  NUMBER coeff_float0;
  NUMBER coeff_float0_x8[8];
  NUMBER coeff_float1;
  u64 coeff_idx;
  CUSTOM custom;
  u32 file_idx;
  u8 *filename_base;
  u8 *filename_ptr;
  FIXED fixed;
  FILE *handle_in;
  CUSTOM *monomial_custom_x8_list_base;
  FIXED *monomial_fixed_x8_list_base;
  NUMBER *monomial_float_x8_list_base;
  u64 monomial_idx;
  NUMBER number;
  u64 number_in_idx;
  number_in_unpadded_t *number_in_list_base;
  u64 pathname_size;
  u64 sample_idx;
  u64 sample_idx_max;
  CUSTOM *sample_custom_x8_list_base;
  FIXED *sample_fixed_x8_list_base;
  NUMBER *sample_float_x8_list_base;
  NUMBER *sample_list_base;
  NUMBER_IN sample_in;
  u8 status;
  u32 thread_idx;
  u64 time_in_ms_end;
  NUMBER time_in_ms_per_transform;
  u64 time_in_ms_start;
  u32 trial_idx;

  print_intro("Verifier");
  coeff_float_x8_list_base=(NUMBER *)(string_create(COEFF_COUNT*8*sizeof(NUMBER)));
  status=!coeff_float_x8_list_base;
  coeff_fixed_x8_list_base=(FIXED *)(string_create(COEFF_COUNT*8*sizeof(FIXED)));
  status|=!coeff_fixed_x8_list_base;
  coeff_custom_x8_list_base=(CUSTOM *)(string_create(COEFF_COUNT*8*sizeof(CUSTOM)));
  status|=!coeff_custom_x8_list_base;
  monomial_float_x8_list_base=(NUMBER *)(string_create(MONOMIAL_COUNT*8*sizeof(NUMBER)));
  status|=!monomial_float_x8_list_base;
  monomial_fixed_x8_list_base=(FIXED *)(string_create(MONOMIAL_COUNT*8*sizeof(FIXED)));
  status|=!monomial_fixed_x8_list_base;
  monomial_custom_x8_list_base=(CUSTOM *)(string_create(MONOMIAL_COUNT*8*sizeof(CUSTOM)));
  status|=!monomial_custom_x8_list_base;
/*
sample_list_base must hold all (NUMBER)s for all SAMPLE_COUNT samples corresponding to all COEFF_COUNT basis vectors.
*/
  sample_list_base=(NUMBER *)(string_create(SAMPLE_COUNT*COEFF_COUNT*sizeof(NUMBER)));
  status|=!sample_list_base;
  sample_float_x8_list_base=(NUMBER *)(string_create(SAMPLE_COUNT*8*sizeof(NUMBER)));
  status|=!sample_float_x8_list_base;
  sample_fixed_x8_list_base=(FIXED *)(string_create(SAMPLE_COUNT*8*sizeof(FIXED)));
  status|=!sample_fixed_x8_list_base;
  sample_custom_x8_list_base=(CUSTOM *)(string_create(SAMPLE_COUNT*8*sizeof(CUSTOM)));
  status|=!sample_custom_x8_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  print("Reading orthobasis vector files...\n");
  pathname_size=strlen(pathname_base);
/*
Set filename_base to the base of a filename containing: (the pathname at pathname_base)+(9 decimal digits for the orthobasis number)+".bin"+NULL.
*/
  filename_base=string_create(pathname_size+9+1+3+1);
  number_in_list_base=(number_in_unpadded_t *)(string_create(SAMPLE_COUNT*sizeof(number_in_unpadded_t)));
  if(!(sample_list_base&&filename_base&&number_in_list_base)){
    print_error_exit_out_of_memory();
  }
/*
Point filename_base to first digit of the (9 decimal digits for the orthobasis number) number located at the end of *filename_base.
*/
  memcpy(filename_base,(const char *)(pathname_base),pathname_size);
  filename_ptr=filename_base+pathname_size;
/*
Append "bin"+NULL.
*/
  filename_ptr[0xA]='b';
  filename_ptr[0xB]='i';
  filename_ptr[0xC]='n';
  filename_ptr[0xD]='\x00';
  file_idx=0;
  sample_idx=0;
  do{
    snprintf(filename_ptr,9+1,"%09d",file_idx);
/*
Replace the "." that sprintf just overwrote.
*/
    filename_ptr[9]='.';
    handle_in=file_open(filename_base,FILE_READ);
    if(!handle_in){
      print_error_string_exit("Cannot read ",filename_base);
    }
    status=file_read(SAMPLE_COUNT*sizeof(number_in_unpadded_t),(u8 *)(number_in_list_base),handle_in);
    if(status){
      printf("%s:\n",filename_base);
      print_error_value_exit("Could not read expected size of %d bytes",SAMPLE_COUNT*sizeof(number_in_unpadded_t));
    }
    file_close(&handle_in);
/*
Convert (NUMBER_IN)s from the source file to (NUMBER)s, transposing them into a matrix SAMPLE_COUNT wide by COEFF_COUNT high. (Thus successive memory addresses correspond first to successive sample indexes, and then to successive coefficients.)
*/
    for(number_in_idx=0;number_in_idx<SAMPLE_COUNT;number_in_idx++){
      NUMBER_IN_PAD(&number_in_list_base[number_in_idx],sample_in);
      sample_list_base[sample_idx]=NUMBER_MAKE(sample_in);
      sample_idx++;
    }
    file_idx++;
  }while(file_idx!=COEFF_COUNT);
  string_free(number_in_list_base);
  string_free(filename_base);
  orthofloat_init(monomial_float_x8_list_base);
/*
We don't need to run orthofixed_init() or orthocustom_init() because we'll just convert the required data from *monomial_float_x8_list_base (which will be used as X1 at first, and only later as X8).
*/
  file_idx=0;
  sample_idx=0;
  do{
    memcpy((void *)(&monomial_float_x8_list_base[1]),(const void*)(&sample_list_base[sample_idx]),SAMPLE_COUNT*sizeof(NUMBER));
    sample_idx+=SAMPLE_COUNT;
    orthofloat_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
/*
All coefficients should be 0, except for the one at file_idx, which should be 1, because we're transforming an orthobasis vector itself. Subtract 1 from this coefficient so that all coefficicents now simply represent error values.
*/
    coeff_float_x8_list_base[file_idx]=DIFFERENCE(NUMBER_MAKE(coeff_float_x8_list_base[file_idx]),NUMBER_MAKE(1));
    SET_ZERO(coeff_float_error_max);
    SET_ZERO(coeff_float_error_mean_squared);
    for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx++){
      coeff_error=MAGNITUDE(coeff_float_x8_list_base[coeff_idx]);
      if(coeff_error>coeff_float_error_max){
        coeff_float_error_max=coeff_error;
      }
      coeff_float_error_mean_squared=SUM(coeff_float_error_mean_squared,PRODUCT(coeff_error,coeff_error));
    }
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      number=monomial_float_x8_list_base[monomial_idx];
      monomial_fixed_x8_list_base[monomial_idx]=FIXED_FROM_NUMBER(number);
    }
    orthofixed_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
    for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx++){
      fixed=coeff_fixed_x8_list_base[coeff_idx];
      coeff_float_x8_list_base[coeff_idx]=FIXED_TO_NUMBER(fixed);
    }
/*
Repeat the error analysis above, for fixed-point.
*/
    coeff_float_x8_list_base[file_idx]=DIFFERENCE(NUMBER_MAKE(coeff_float_x8_list_base[file_idx]),NUMBER_MAKE(1));
    SET_ZERO(coeff_fixed_error_max);
    SET_ZERO(coeff_fixed_error_mean_squared);
    for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx++){
      coeff_fixed_error=MAGNITUDE(coeff_float_x8_list_base[coeff_idx]);
      if(coeff_fixed_error>coeff_fixed_error_max){
        coeff_fixed_error_max=coeff_fixed_error;
      }
      coeff_fixed_error_mean_squared=SUM(coeff_fixed_error_mean_squared,PRODUCT(coeff_fixed_error,coeff_fixed_error));
    }
/*
Repeat the fixed-point exercise above, but for custom fixed-point.
*/
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      number=monomial_float_x8_list_base[monomial_idx];
      CUSTOM_FROM_NUMBER(number,custom);
      monomial_custom_x8_list_base[monomial_idx]=custom;
    }
    orthocustom_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
    for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx++){
      custom=coeff_custom_x8_list_base[coeff_idx];
      coeff_float_x8_list_base[coeff_idx]=CUSTOM_TO_NUMBER(custom);
    }
/*
Repeat the error analysis above, for custom fixed-point.
*/
    coeff_float_x8_list_base[file_idx]=DIFFERENCE(NUMBER_MAKE(coeff_float_x8_list_base[file_idx]),NUMBER_MAKE(1));
    SET_ZERO(coeff_custom_error_max);
    SET_ZERO(coeff_custom_error_mean_squared);
    for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx++){
      coeff_custom_error=MAGNITUDE(coeff_float_x8_list_base[coeff_idx]);
      if(coeff_custom_error>coeff_custom_error_max){
        coeff_custom_error_max=coeff_custom_error;
      }
      coeff_custom_error_mean_squared=SUM(coeff_custom_error_mean_squared,PRODUCT(coeff_custom_error,coeff_custom_error));
    }
    file_idx++;
  }while(file_idx!=COEFF_COUNT);
/*
Divide coeff_float_error_mean_squared by COEFF_COUNT twice, so we can avoid some annoying overflow cases. (We have COEFF_COUNT coefficients for each of COEFF_COUNT basis vectors, hence the square.)
*/
  coeff_float_error_mean_squared=RATIO(coeff_float_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  coeff_float_error_mean_squared=RATIO(coeff_float_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  coeff_fixed_error_mean_squared=RATIO(coeff_fixed_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  coeff_fixed_error_mean_squared=RATIO(coeff_fixed_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  coeff_custom_error_mean_squared=RATIO(coeff_custom_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  coeff_custom_error_mean_squared=RATIO(coeff_custom_error_mean_squared,NUMBER_MAKE(COEFF_COUNT));
  printf("\nError info is only relevant for orthonormal (not merely orthogonal, and\ncertainly not nonorthogonal) transforms:\n\n");
  DEBUG_NUMBER("coeff_floating_error_max",coeff_float_error_max);
  DEBUG_NUMBER("coeff_floating_error_mean_squared",coeff_float_error_mean_squared);
  DEBUG_NUMBER("coeff_fixed_error_max",coeff_fixed_error_max);
  DEBUG_NUMBER("coeff_fixed_error_mean_squared",coeff_fixed_error_mean_squared);
  DEBUG_NUMBER("coeff_custom_error_max",coeff_custom_error_max);
  DEBUG_NUMBER("coeff_custom_error_mean_squared",coeff_custom_error_mean_squared);
  print("\nFinding min and max possible coefficients for custom fixed-point,\nfixed-point, and floating-point (may take several minutes)...\n\n");
  CUSTOM_SET_ZERO(coeff_custom_min);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    CUSTOM_SET_ZERO(custom);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      CUSTOM_SET_NEGATIVE_ONE(custom);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
      orthocustom_x8_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_custom0_x8[thread_idx]=coeff_custom_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      CUSTOM_SET_ONE_MINUS_EPSILON(custom);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
      orthocustom_x8_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_custom0=coeff_custom0_x8[thread_idx];
          coeff_custom1=coeff_custom_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(CUSTOM_IS_LESS(coeff_custom0,coeff_custom1)){
            CUSTOM_SET_NEGATIVE_ONE(custom);
            monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
            coeff_custom1=coeff_custom0;
          }
          if(CUSTOM_IS_LESS(coeff_custom1,coeff_custom_min)){
            coeff_custom_min=coeff_custom1;
          }
        }
      }
    }
  }
  DEBUG_U32("coeff_custom_min",coeff_custom_min);
  CUSTOM_SET_ZERO(coeff_custom_max);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    CUSTOM_SET_ZERO(custom);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      CUSTOM_SET_NEGATIVE_ONE(custom);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
      orthocustom_x8_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_custom0_x8[thread_idx]=coeff_custom_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      CUSTOM_SET_ONE_MINUS_EPSILON(custom);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
      }
      orthocustom_x8_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_custom0=coeff_custom0_x8[thread_idx];
          coeff_custom1=coeff_custom_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(CUSTOM_IS_GREATER(coeff_custom0,coeff_custom1)){
            CUSTOM_SET_NEGATIVE_ONE(custom);
            monomial_custom_x8_list_base[(monomial_idx<<3)+thread_idx]=custom;
            coeff_custom1=coeff_custom0;
          }
          if(CUSTOM_IS_GREATER(coeff_custom1,coeff_custom_max)){
            coeff_custom_max=coeff_custom1;
          }
        }
      }
    }
  }
  DEBUG_U32("coeff_custom_max",coeff_custom_max);
  FIXED_SET_ZERO(coeff_fixed_min);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    FIXED_SET_ZERO(fixed);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      FIXED_SET_NEGATIVE_ONE(fixed);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
      orthofixed_x8_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_fixed0_x8[thread_idx]=coeff_fixed_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      FIXED_SET_ONE_MINUS_EPSILON(fixed);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
      orthofixed_x8_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_fixed0=coeff_fixed0_x8[thread_idx];
          coeff_fixed1=coeff_fixed_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(FIXED_IS_LESS(coeff_fixed0,coeff_fixed1)){
            FIXED_SET_NEGATIVE_ONE(fixed);
            monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
            coeff_fixed1=coeff_fixed0;
          }
          if(FIXED_IS_LESS(coeff_fixed1,coeff_fixed_min)){
            coeff_fixed_min=coeff_fixed1;
          }
        }
      }
    }
  }
  DEBUG_U64("coeff_fixed_min",coeff_fixed_min);
  FIXED_SET_ZERO(coeff_fixed_max);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    FIXED_SET_ZERO(fixed);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      FIXED_SET_NEGATIVE_ONE(fixed);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
      orthofixed_x8_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_fixed0_x8[thread_idx]=coeff_fixed_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      FIXED_SET_ONE_MINUS_EPSILON(fixed);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
      }
      orthofixed_x8_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_fixed0=coeff_fixed0_x8[thread_idx];
          coeff_fixed1=coeff_fixed_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(FIXED_IS_GREATER(coeff_fixed0,coeff_fixed1)){
            FIXED_SET_NEGATIVE_ONE(fixed);
            monomial_fixed_x8_list_base[(monomial_idx<<3)+thread_idx]=fixed;
            coeff_fixed1=coeff_fixed0;
          }
          if(FIXED_IS_GREATER(coeff_fixed1,coeff_fixed_max)){
            coeff_fixed_max=coeff_fixed1;
          }
        }
      }
    }
  }
  DEBUG_U64("coeff_fixed_max",coeff_fixed_max);
  SET_ZERO(coeff_float_min);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    SET_ZERO(number);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      SET_NEGATIVE_ONE(number);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
      orthofloat_x8_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_float0_x8[thread_idx]=coeff_float_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      SET_ONE(number);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
      orthofloat_x8_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_float0=coeff_float0_x8[thread_idx];
          coeff_float1=coeff_float_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(IS_LESS(coeff_float0,coeff_float1)){
            SET_NEGATIVE_ONE(number);
            monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
            coeff_float1=coeff_float0;
          }
          if(IS_LESS(coeff_float1,coeff_float_min)){
            coeff_float_min=coeff_float1;
          }
        }
      }
    }
  }
  DEBUG_NUMBER("coeff_float_min",coeff_float_min);
  SET_ZERO(coeff_float_max);
  for(coeff_idx=0;coeff_idx<=COEFF_IDX_MAX;coeff_idx+=8){
    SET_ZERO(number);
    for(monomial_idx=0;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
    }
    for(monomial_idx=1;monomial_idx<=SAMPLE_COUNT;monomial_idx++){
      SET_NEGATIVE_ONE(number);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
      orthofloat_x8_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_float0_x8[thread_idx]=coeff_float_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
        }
      }
      SET_ONE(number);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
      }
      orthofloat_x8_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
      for(thread_idx=0;thread_idx<=7;thread_idx++){
        if((coeff_idx+thread_idx)<=COEFF_IDX_MAX){
          coeff_float0=coeff_float0_x8[thread_idx];
          coeff_float1=coeff_float_x8_list_base[((coeff_idx+thread_idx)<<3)+thread_idx];
          if(IS_GREATER(coeff_float0,coeff_float1)){
            SET_NEGATIVE_ONE(number);
            monomial_float_x8_list_base[(monomial_idx<<3)+thread_idx]=number;
            coeff_float1=coeff_float0;
          }
          if(IS_GREATER(coeff_float1,coeff_float_max)){
            coeff_float_max=coeff_float1;
          }
        }
      }
    }
  }
  DEBUG_NUMBER("coeff_float_max",coeff_float_max);
  print("\nDone.\n");
  string_free(sample_custom_x8_list_base);
  string_free(sample_fixed_x8_list_base);
  string_free(sample_float_x8_list_base);
  string_free(sample_list_base);
  string_free(monomial_custom_x8_list_base);
  string_free(monomial_fixed_x8_list_base);
  string_free(monomial_float_x8_list_base);
  string_free(coeff_custom_x8_list_base);
  string_free(coeff_fixed_x8_list_base);
  string_free(coeff_float_x8_list_base);
  return 0;
}
