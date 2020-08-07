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
Orthohack Floating-Point-X8 Transform
*/
#ifndef CONSTANTS_DEFINED
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include "orthoconstant.h"
#endif
/*
This filename must point to the captured output of post.c. It contains the NUMBER_LIST, BINOMIAL_LIST, and TRANSFORM_STRING necessary to perform the transform.
*/
#ifndef ORTHOHACK_DEFINED
#include "orthohack.h"
#endif
#ifndef NUMBER_COMPONENT_IDX_MAX
#include "orthodim.h"
#endif
#ifndef NUMBER_DEFINED
#include "orthonumber.h"
#endif
#ifndef TABLE_DEFINED
#include "orthotable.c"
#endif
#ifndef TABLE_FLOAT_DEFINED
#include "orthofloat_table.c"
#endif

void
orthofloat_x8_init(NUMBER monomial_x8_list_base[MONOMIAL_COUNT*8],NUMBER sample_list_x8_base[SAMPLE_IDX_MAX*8]){
/*
Prepare for a floating-point-X8 transform.

In:

  *sample_list_x8_base is readable for (SAMPLE_IDX_MAX*8) (NUMBER)s. Successive sets of SAMPLE_IDX_MAX (NUMBER)s belong to successive transform domains. The first sample should be actual sample data, unlike *monomial_x8_list_base, whose first item is always set to 0. Note the difference in topology between "x8_list", in which threads are interleaved, and "list_x8" in which entire complete threads are concatenated.

  *monomial_x8_list_base is writable for (MONOMIAL_COUNT*8) (NUMBER)s.

Out:

  *monomial_x8_list_base is formatted so as to satisfy the assumptions of a floating-point-X8 transform. Namely, the first (SAMPLE_IDX_MAX+1) sets of 8 monomials are initialized to: 8 0s, followed by (SAMPLE_IDX_MAX*8) samples from *sample_list_base, such that successive monomials correspond to the successive transforms, modulo 8.
*/
  NUMBER number;
  u64 monomial_idx;
  u64 sample_idx;
  u8 thread_idx;

  SET_ZERO(number);
  monomial_idx=0;
  do{
    monomial_x8_list_base[monomial_idx]=0;
    monomial_idx++;
  }while(monomial_idx<=7);
  sample_idx=0;
  do{
    for(thread_idx=0;thread_idx<=7;thread_idx++){
      number=sample_list_x8_base[sample_idx];
      sample_idx+=SAMPLE_IDX_MAX;
      monomial_x8_list_base[monomial_idx]=number;
      monomial_idx++;
    }
    sample_idx-=(SAMPLE_IDX_MAX*8)-1;
  }while(sample_idx!=SAMPLE_IDX_MAX);
  return;
}

void
orthofloat_x8_end(NUMBER coeff_list_base[COEFF_COUNT*8],NUMBER coeff_x8_list_base[COEFF_COUNT*8]){
/*
Issue the results of a floating-point-X8 transform.

In:

  *coeff_x8_list_base is the output of orthofloat_x8_transform().

  *coeff_list_base is writable for (COEFF_COUNT*8) (NUMBER)s. 

Out:

  *coeff_list_base contains 8 successive sets of COEFF_COUNT (NUMBER)s, each set corresponding to the same set in orthofloat_x8_init():In:sample_list_base.
*/
  u64 coeff_idx;
  u64 coeff_x8_idx;
  NUMBER number;
  u64 monomial_idx;
  u8 thread_idx;

  coeff_x8_idx=0;
  coeff_idx=0;
  do{
    for(thread_idx=0;thread_idx<=7;thread_idx++){
      number=coeff_x8_list_base[coeff_x8_idx];
      coeff_x8_idx++;
      coeff_list_base[coeff_idx]=number;
      coeff_idx+=COEFF_COUNT;
    }
    coeff_idx-=(COEFF_COUNT*8)-1;
  }while(coeff_idx!=COEFF_COUNT);
  return;
}

void
orthofloat_x8_transform(NUMBER coeff_x8_list_base[COEFF_COUNT*8],NUMBER monomial_x8_list_base[MONOMIAL_COUNT*8]){
/*
Perform the floating-point-X8 transform optimized by pre and post at the precision defined by PRECISION_INTERNAL in orthohack.h, which in turn results in the definition of a NUMBER by orthofloat.h.

In:

  *coeff_x8_list_base is writable for (COEFF_COUNT*8) (NUMBER)s.

  *monomial_x8_list_base is orthofloat_x8_init():Out:monomial_x8_list_base.

Out:

  *coeff_x8_list_base contains the coefficients, expressed as (NUMBER)s, and arranged in the order corresponding to the ascending filenames which were originally fed into pre, such that each 8 successive coefficients correspond to successive transforms.

  *monomial_x8_list_base is unchanged where it was defined on input (the first ((SAMPLE_IDX_MAX+1)*8) (NUMBER)s). Otherwise undefined.
*/
  u64 alias_count;
  NUMBER coeff;
  u64 coeff_idx;
  NUMBER denominator;
  u64 denominator_idx;
  NUMBER monomial;
  u64 monomial_count;
  u64 monomial_idx;
  NUMBER numerator;
  u64 numerator_idx;
  u8 packed_bit_count;
  u8 packed_shift;
  u64 packed_u64;
  u64 packed_u64_idx;
  NUMBER ratio;
  u64 ratio_idx;
  u8 sign;
  u64 term_count;

  SET_ZERO(ratio);
  for(coeff_idx=0;coeff_idx<=((COEFF_COUNT*8)-1);coeff_idx++){
    coeff_x8_list_base[coeff_idx]=ratio;
  }
  monomial_idx=(SAMPLE_IDX_MAX+1)<<3;
  packed_u64_idx=0;
  packed_bit_count=0;
  packed_shift=0;
  packed_u64=0;
  term_count=BINOMIAL_COUNT;
  do{
    #include "orthobinomial_x8.c"
    sign=ratio_idx&((1<<(NUMBER_COMPONENT_IDX_MAX+1))-1);
    ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
    ratio=((NUMBER *)(number_list_base))[ratio_idx];
    numerator_idx<<=3;
    denominator_idx<<=3;
    ratio=SIGN_SET(ratio,sign);
/*
Thread 0.
*/
    numerator=monomial_x8_list_base[numerator_idx];
    denominator=monomial_x8_list_base[denominator_idx];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx]=monomial;
/*
Thread 1.
*/
    numerator=monomial_x8_list_base[numerator_idx+1];
    denominator=monomial_x8_list_base[denominator_idx+1];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+1]=monomial;
/*
Thread 2.
*/
    numerator=monomial_x8_list_base[numerator_idx+2];
    denominator=monomial_x8_list_base[denominator_idx+2];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+2]=monomial;
/*
Thread 3.
*/
    numerator=monomial_x8_list_base[numerator_idx+3];
    denominator=monomial_x8_list_base[denominator_idx+3];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+3]=monomial;
/*
Thread 4.
*/
    numerator=monomial_x8_list_base[numerator_idx+4];
    denominator=monomial_x8_list_base[denominator_idx+4];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+4]=monomial;
/*
Thread 5.
*/
    numerator=monomial_x8_list_base[numerator_idx+5];
    denominator=monomial_x8_list_base[denominator_idx+5];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+5]=monomial;
/*
Thread 6.
*/
    numerator=monomial_x8_list_base[numerator_idx+6];
    denominator=monomial_x8_list_base[denominator_idx+6];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+6]=monomial;
/*
Thread 7.
*/
    numerator=monomial_x8_list_base[numerator_idx+7];
    denominator=monomial_x8_list_base[denominator_idx+7];
    monomial=PRODUCT(ratio,numerator);
    monomial=SUM(monomial,denominator);
    monomial_x8_list_base[monomial_idx+7]=monomial;
    monomial_idx+=8;
    term_count--;
  }while(term_count);
  monomial_idx=0;
  packed_u64_idx=0;
  packed_bit_count=0;
  packed_shift=0;
  packed_u64=0;
  do{
    #include "orthomonomial_zero_x8.c"
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
        ratio_idx>>=NUMBER_COMPONENT_IDX_MAX+1;
        ratio=((NUMBER *)(number_list_base))[ratio_idx];
        coeff_idx<<=3;
        ratio=SIGN_SET(ratio,sign);
/*
Thread 0.
*/
        monomial=monomial_x8_list_base[monomial_idx];
        coeff=coeff_x8_list_base[coeff_idx];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx]=coeff;
/*
Thread 1.
*/
        monomial=monomial_x8_list_base[monomial_idx+1];
        coeff=coeff_x8_list_base[coeff_idx+1];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+1]=coeff;
/*
Thread 2.
*/
        monomial=monomial_x8_list_base[monomial_idx+2];
        coeff=coeff_x8_list_base[coeff_idx+2];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+2]=coeff;
/*
Thread 3.
*/
        monomial=monomial_x8_list_base[monomial_idx+3];
        coeff=coeff_x8_list_base[coeff_idx+3];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+3]=coeff;
/*
Thread 4.
*/
        monomial=monomial_x8_list_base[monomial_idx+4];
        coeff=coeff_x8_list_base[coeff_idx+4];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+4]=coeff;
/*
Thread 5.
*/
        monomial=monomial_x8_list_base[monomial_idx+5];
        coeff=coeff_x8_list_base[coeff_idx+5];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+5]=coeff;
/*
Thread 6.
*/
        monomial=monomial_x8_list_base[monomial_idx+6];
        coeff=coeff_x8_list_base[coeff_idx+6];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+6]=coeff;
/*
Thread 7.
*/
        monomial=monomial_x8_list_base[monomial_idx+7];
        coeff=coeff_x8_list_base[coeff_idx+7];
        monomial=PRODUCT(ratio,monomial);
        coeff=SUM(coeff,monomial);
        coeff_x8_list_base[coeff_idx+7]=coeff;
        alias_count--;
      }while(alias_count);
      monomial_idx+=8;     
    }
  }while(monomial_idx<=(MONOMIAL_IDX_MAX<<3));
  return;
}
