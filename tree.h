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
TYPEDEF_START
  u64 coeff_idx;
  u64 ratio_idx;
  u64 monomial_idx;
TYPEDEF_END(alias_t)

TYPEDEF_START
  u16 number[NUMBER_IN_U16_IDX_MAX+1];
TYPEDEF_END(number_in_unpadded_t)

TYPEDEF_START
  u16 number[NUMBER_OUT_U16_IDX_MAX+1];
TYPEDEF_END(number_out_unpadded_t)

TYPEDEF_START
  u64 denominator_idx;
  u64 numerator_idx;
  number_in_unpadded_t ratio;
TYPEDEF_END(binomial_in_t)

TYPEDEF_START
  u64 denominator_idx;
  u64 numerator_idx;
  number_out_unpadded_t ratio;
TYPEDEF_END(binomial_out_t)

TYPEDEF_START
  u64 denominator_idx;
  u64 numerator_idx;
  NUMBER ratio;
TYPEDEF_END(binomial_t)

TYPEDEF_START
  u64 denominator_idx;
  u64 numerator_idx;
  u64 ratio_idx;
  NUMBER ratio;
TYPEDEF_END(binomial_remap_t)

TYPEDEF_START
  u64 sample_idx;
  number_in_unpadded_t ratio;
TYPEDEF_END(monomial_in_t)

TYPEDEF_START
  u64 sample_idx;
  number_out_unpadded_t ratio;
TYPEDEF_END(monomial_out_t)

TYPEDEF_START
  u64 sample_idx;
  NUMBER ratio;
TYPEDEF_END(monomial_t)

TYPEDEF_START
  u64 sample_idx;
  u64 ratio_idx;
  NUMBER ratio;
TYPEDEF_END(monomial_remap_t)
