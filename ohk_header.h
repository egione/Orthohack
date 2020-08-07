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
  u64 signature;
#define ORTHOHACK_FILE_SIGNATURE 0x6984E354327FC0E1ULL
/*
size_following is the number of bytes in the entire file after size_following.
*/
  u64 size_following;
/*
coeff_idx_max is the number of coefficients in the output of the orthobasis transform, minus 1.
*/
  u64 coeff_idx_max;
/*
sample_idx_max is the number of spatial samples in the input of the orthobasis transform, minus 1.
*/
  u64 sample_idx_max;
/*
binomial_idx_max is the number of binomials in the binomial list (located after the monomials), minus 1. The maximum number of monomials required to compute a single coefficient is (sample_idx_max+binomial_idx_max+1).
*/
  u64 binomial_idx_max;
/*
precision_zero_limit is a double precision approximation of PRECISION_ZERO_LIMIT, purely for documentation purposes.
*/
  double precision_zero_limit;
/*
precision_ratio_limit_minus_1 is a double precision approximation of (PRECISION_RATIO_LIMIT-1), purely for documentation purposes.
*/
  double precision_ratio_limit_minus_1;
/*
number_component_idx_max taken from NUMBER_COMPONENT_IDX_MAX in number.h.
*/
  u8 number_component_idx_max;
/*
number_u16_idx_max is taken from NUMBER_IN_U16_IDX_MAX in number.h.
*/
  u8 number_u16_idx_max;
/*
reserved_zero is 0.
*/
  u16 reserved_zero[3];
TYPEDEF_END(orthohack_header_t)

