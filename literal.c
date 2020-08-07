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
#include "orthohack.h"
#include "orthodim.h"
#include "orthonumber.h"
#include "orthofloat.c"
#include "debug.c"
#include "string.c"
#include "print.c"

int
main(int argc, char *argv[]){
  u64 alias_count;
  u64 coeff_idx;
  u64 denominator_idx;
  u64 monomial_count;
  u64 monomial_idx;
  u64 numerator_idx;
  u8 packed_bit_count;
  u8 packed_shift;
  u64 packed_u64;
  u64 packed_u64_idx;
  u64 ratio_idx;
  u8 sign;
  u64 term_count;

  printf("/*\n");
  print_intro("Transform Code Literalizer");
  printf("*/\n");
  monomial_idx=SAMPLE_IDX_MAX+1;
  packed_u64_idx=0;
  packed_bit_count=0;
  packed_shift=0;
  packed_u64=0;
  term_count=BINOMIAL_COUNT;
  do{
/*
Unpack denominator_idx, numerator_idx, and ratio_idx one at a time. Adapted from orthobinomial_x1.c.
*/
    if(packed_bit_count<MONOMIAL_IDX_BIT_COUNT){
      packed_u64+=((binomial_list_base[packed_u64_idx]>>packed_shift)<<packed_bit_count);
      if(packed_shift<packed_bit_count){
        packed_shift+=U64_BITS-packed_bit_count;
      }else{
        packed_u64_idx++;
        if(packed_shift>packed_bit_count){
          packed_bit_count+=U64_BITS-packed_shift;
          packed_u64+=binomial_list_base[packed_u64_idx]<<packed_bit_count;
          packed_shift=U64_BITS-packed_bit_count;
        }else{
          packed_shift=0;        
        }
      }      
      packed_bit_count=U64_BITS;
    }
    denominator_idx=packed_u64&MONOMIAL_IDX_MASK;
    packed_u64>>=MONOMIAL_IDX_BIT_COUNT;
    packed_bit_count-=MONOMIAL_IDX_BIT_COUNT;
    if(packed_bit_count<MONOMIAL_IDX_BIT_COUNT){
      packed_u64+=((binomial_list_base[packed_u64_idx]>>packed_shift)<<packed_bit_count);
      if(packed_shift<packed_bit_count){
        packed_shift+=U64_BITS-packed_bit_count;
      }else{
        packed_u64_idx++;
        if(packed_shift>packed_bit_count){
          packed_bit_count+=U64_BITS-packed_shift;
          packed_u64+=binomial_list_base[packed_u64_idx]<<packed_bit_count;
          packed_shift=U64_BITS-packed_bit_count;
        }else{
          packed_shift=0;        
        }
      }
      packed_bit_count=U64_BITS;
    }
    numerator_idx=packed_u64&MONOMIAL_IDX_MASK;
    packed_u64>>=MONOMIAL_IDX_BIT_COUNT;
    packed_bit_count-=MONOMIAL_IDX_BIT_COUNT;
    if(packed_bit_count<NUMBER_IDX_BIT_COUNT){
      packed_u64+=((binomial_list_base[packed_u64_idx]>>packed_shift)<<packed_bit_count);
      if(packed_shift<packed_bit_count){
        packed_shift+=U64_BITS-packed_bit_count;
      }else{
        packed_u64_idx++;
        if(packed_shift>packed_bit_count){
          packed_bit_count+=U64_BITS-packed_shift;
          packed_u64+=binomial_list_base[packed_u64_idx]<<packed_bit_count;
          packed_shift=U64_BITS-packed_bit_count;
        }else{
          packed_shift=0;
        }
      }      
      packed_bit_count=U64_BITS;
    }
    ratio_idx=packed_u64&NUMBER_IDX_MASK;
    packed_u64>>=NUMBER_IDX_BIT_COUNT;
    packed_bit_count-=NUMBER_IDX_BIT_COUNT;
    sign=ratio_idx&((1<<(NUMBER_COMPONENT_IDX_MAX+1))-1);
    if(sign==0){
      printf("A");
    }else{
      printf("B");
    }
    ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
    printf("(%d,%d,%d,%d);\n",(u32)(ratio_idx),(u32)(numerator_idx),(u32)(denominator_idx),(u32)(monomial_idx));
    monomial_idx++;
    term_count--;
  }while(term_count);
  monomial_idx=0;
  packed_u64_idx=0;
  packed_bit_count=0;
  packed_shift=0;
  packed_u64=0;
  do{
    #include "orthomonomial_zero_x1.c"
/*
Process monomial_count nonzero monomials.
*/
    while(monomial_count--){
      if(packed_bit_count<ALIAS_COUNT_BIT_COUNT){
        packed_u64+=((transform_string_base[packed_u64_idx]>>packed_shift)<<packed_bit_count);
        if(packed_shift<packed_bit_count){
          packed_shift+=U64_BITS-packed_bit_count;
        }else{
          packed_u64_idx++;
          if(packed_shift>packed_bit_count){
            packed_bit_count+=U64_BITS-packed_shift;
            packed_u64+=transform_string_base[packed_u64_idx]<<packed_bit_count;
            packed_shift=U64_BITS-packed_bit_count;
          }else{
            packed_shift=0;        
          }
        }      
        packed_bit_count=U64_BITS;
      }
      alias_count=packed_u64&ALIAS_COUNT_MASK;
      packed_u64>>=ALIAS_COUNT_BIT_COUNT;
      packed_bit_count-=ALIAS_COUNT_BIT_COUNT;
/*
Process alias_count (ratio_idx, coeff_idx) pairs.
*/
      do{
        #include "orthomonomial_nonzero.c"
        sign=ratio_idx&((1<<(NUMBER_COMPONENT_IDX_MAX+1))-1);
        if(sign==0){
          printf("C");
        }else{
          printf("D");
        }
        ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
        printf("(%d,%d,%d);\n",(u32)(ratio_idx),(u32)(monomial_idx),(u32)(coeff_idx));
        alias_count--;
      }while(alias_count);
      monomial_idx++;     
    }
  }while(monomial_idx<=MONOMIAL_IDX_MAX);
  return 0;
}
