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
Nonzero monomial unpacker for all transforms.
*/
#if (NUMBER_IDX_BIT_COUNT+COEFF_IDX_BIT_COUNT)<=U64_BIT_MAX
        if(packed_bit_count<(NUMBER_IDX_BIT_COUNT+COEFF_IDX_BIT_COUNT)){
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
        ratio_idx=packed_u64&NUMBER_IDX_MASK;
        packed_u64>>=NUMBER_IDX_BIT_COUNT;
        coeff_idx=packed_u64&COEFF_IDX_MASK;
        packed_u64>>=COEFF_IDX_BIT_COUNT;
        packed_bit_count-=NUMBER_IDX_BIT_COUNT+COEFF_IDX_BIT_COUNT;
#else
        if(packed_bit_count<NUMBER_IDX_BIT_COUNT){
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
        ratio_idx=packed_u64&NUMBER_IDX_MASK;
        packed_u64>>=NUMBER_IDX_BIT_COUNT;
        packed_bit_count-=NUMBER_IDX_BIT_COUNT;
        if(packed_bit_count<COEFF_IDX_BIT_COUNT){
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
        coeff_idx=packed_u64&COEFF_IDX_MASK;
        packed_u64>>=COEFF_IDX_BIT_COUNT;
        packed_bit_count-=COEFF_IDX_BIT_COUNT;
#endif
