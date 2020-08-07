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
u8
pack(u8 *packed_bit_count_base,u64 *packed_hi_base,u64 *packed_lo_base,u64 value,u8 value_bit_count){
/*
Pack a sub-64-bit bitfield into a 128-bit hull.

In:

  *packed_bit_count_base is the base of the current bit count, initially 0, on [0,63].

  *packed_hi_base is the base of the high 64 bits of the 128-bit hull.

  *packed_lo_base is the base of the low 64 bits of the 128-bit hull.

  value is the value scroll into the hull, scrolling down from the MSB of the latter.

  value_bit_count is the number of bits in value to scroll into the hull, [1,63].

Out:

  Returns 1 if *packed_bit_count_base wrapped 64.

  *packed_bit_count_base is its value on input, plus value_bit_count, modulo 64.

  *packed_hi_base is updated.

  *packed_lo_base is updated.
*/
  u8 status;
  u64 packed_hi;
  u64 packed_lo;
  u8 value_bit_count_negative;

  packed_hi=*packed_hi_base;
  packed_lo=*packed_lo_base;
  value_bit_count_negative=U64_BITS-value_bit_count;
  packed_lo=(packed_lo>>value_bit_count)+(packed_hi<<value_bit_count_negative);
  packed_hi>>=value_bit_count;
  packed_hi+=value<<value_bit_count_negative;
  value_bit_count+=*packed_bit_count_base;
  status=value_bit_count>>U64_BITS_LOG2;
  value_bit_count&=U64_BIT_MAX;
  *packed_bit_count_base=value_bit_count;
  *packed_lo_base=packed_lo;
  *packed_hi_base=packed_hi;
  return status;
}

u64
pack_output(u8 packed_bit_count,u64 packed_hi,u64 packed_lo){
/*
Output 64 bits of packed data.

In:

  output_idx is the index into *output_list_base at which to store a packed u64.

  (*output_list_base)[output_idx] is writable for size u64.

  packed_bit_count is the number of significant high bits in packed_lo, [0, 63]. This is 64 bits less than the total number of valid bits in packed_hi:packed_lo.

  packed_hi is the high part of the packed hull.

  packed_hi is the low part of the packed hull.

Out:

  Returns the 64 bits of packed_hi:packed_lo which are based at bit (U64_BITS-packed_bit_count).
*/
  if(packed_bit_count){
    packed_lo>>=U64_BITS-packed_bit_count;
  }else{
    packed_lo=0;
  }
  packed_lo+=packed_hi<<packed_bit_count;
  return packed_lo;
}
