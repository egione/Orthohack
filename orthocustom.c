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
/*
Orthohack Custom Fixed-Point Transform
*/
#ifndef CONSTANTS_DEFINED
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include "orthoconstant.h"
#endif
/*
This filename must point to the captured output of post.c. It contains the CUSTOM_LIST, BINOMIAL_LIST, and TRANSFORM_STRING necessary to perform the transform.
*/
#ifndef ORTHOHACK_DEFINED
#include "orthohack.h"
#endif
#ifndef NUMBER_COMPONENT_IDX_MAX
#include "orthodim.h"
#endif
#ifndef CUSTOM_DEFINED
#include "orthocustom.h"
#endif
#ifndef TABLE_DEFINED
#include "orthotable.c"
#endif
#ifndef TABLE_CUSTOM_DEFINED
#include "orthocustom_table.c"
#endif

void
orthocustom_init(CUSTOM monomial_list_base[MONOMIAL_COUNT]){
/*
Prepare for a custom fixed-point transform.

In:

  *monomial_list_base is writable for MONOMIAL_COUNT (CUSTOM)s.

Out:

  *monomial_list_base is formatted so as to satisfy the assumptions of a custom fixed-point transform.
*/
  CUSTOM custom;

  CUSTOM_SET_ZERO(custom);
  *monomial_list_base=custom;
  return;
}

void
orthocustom_transform(CUSTOM coeff_list_base[COEFF_COUNT],CUSTOM monomial_list_base[MONOMIAL_COUNT]){
/*
Perform the custom fixed-point transform optimized by pre and post at the precision defined by PRECISION_CUSTOM in orthohack.h, which in turn results in the definition of a CUSTOM by custom.h.

In:

  *coeff_list_base is writable for COEFF_COUNT (CUSTOM)s.

  *monomial_list_base was initialized with orthofloat_init(). SAMPLE_IDX_MAX (CUSTOM)s starting at index 1 contain the spatial samples to transform, in the same order as in the orthobasis files which were originally fed into pre. Otherwise undefined and writable for MONOMIAL_COUNT (CUSTOM)s.

Out:

  *coeff_list_base contains the coefficients, expressed as (CUSTOM)s, and arranged in the order corresponding to the ascending filenames which were originally fed into pre.

  *monomial_list_base is unchanged where it was defined on input (the first (SAMPLE_IDX_MAX+1) (CUSTOM)s). Otherwise undefined.
*/
  u64 alias_count;
  CUSTOM coeff;
  u64 coeff_idx;
  CUSTOM denominator;
  u64 denominator_idx;
  CUSTOM monomial;
  u64 monomial_count;
  u64 monomial_idx;
  CUSTOM numerator;
  u64 numerator_idx;
  u8 packed_bit_count;
  u8 packed_shift;
  u64 packed_u64;
  u64 packed_u64_idx;
  CUSTOM ratio;
  u64 ratio_idx;
  u8 sign;
  u64 term_count;

  CUSTOM_SET_ZERO(ratio);
  for(coeff_idx=0;coeff_idx<=(COEFF_COUNT-1);coeff_idx++){
    coeff_list_base[coeff_idx]=ratio;
  }
  monomial_idx=SAMPLE_IDX_MAX+1;
  packed_u64_idx=0;
  packed_bit_count=0;
  packed_shift=0;
  packed_u64=0;
  term_count=BINOMIAL_COUNT;
  do{
    #include "orthobinomial_x1.c"
    sign=ratio_idx&((1<<(NUMBER_COMPONENT_IDX_MAX+1))-1);
    ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
    ratio=custom_list_base[ratio_idx];
    CUSTOM_SIGN_SET(ratio,sign,ratio);
    CUSTOM_PRODUCT(ratio,numerator,monomial);
    CUSTOM_SUM(monomial,denominator,monomial);
    monomial_list_base[monomial_idx]=monomial;
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
        monomial=monomial_list_base[monomial_idx];
        sign=ratio_idx&((1<<(NUMBER_COMPONENT_IDX_MAX+1))-1);
        ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
        ratio=custom_list_base[ratio_idx];
        CUSTOM_SIGN_SET(ratio,sign,ratio);
        coeff=coeff_list_base[coeff_idx];
        CUSTOM_PRODUCT(ratio,monomial,monomial);
        CUSTOM_SUM(coeff,monomial,coeff);
        coeff_list_base[coeff_idx]=coeff;
        alias_count--;
      }while(alias_count);
      monomial_idx++;     
    }
  }while(monomial_idx<=MONOMIAL_IDX_MAX);
  return;
}
