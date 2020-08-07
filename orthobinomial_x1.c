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
If you change this code, you must port your changes to literal.c.

Binomial list unpacking for X1 transforms.
*/
#if ((MONOMIAL_IDX_BIT_COUNT<<1)+NUMBER_IDX_BIT_COUNT)<=U64_BIT_MAX
/*
Unpack denominator_idx, numerator_idx, and ratio_idx all at once, so as to form a binomial.
*/
    if(packed_bit_count<((MONOMIAL_IDX_BIT_COUNT<<1)+NUMBER_IDX_BIT_COUNT)){
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
    denominator=monomial_list_base[denominator_idx];
    numerator_idx=packed_u64&MONOMIAL_IDX_MASK;
    packed_u64>>=MONOMIAL_IDX_BIT_COUNT;
    numerator=monomial_list_base[numerator_idx];
    ratio_idx=packed_u64&NUMBER_IDX_MASK;
    packed_u64>>=NUMBER_IDX_BIT_COUNT;
    packed_bit_count-=(MONOMIAL_IDX_BIT_COUNT<<1)+NUMBER_IDX_BIT_COUNT;
#else
/*
Unpack denominator_idx, numerator_idx, and ratio_idx one at a time.
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
    denominator=monomial_list_base[denominator_idx];
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
    numerator=monomial_list_base[numerator_idx];
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
#endif
