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
POPULATION_MIN, which must be at least 2, is the least allowable population of a binomial in order to justify substitution. 2 implies the least redundant and (probably) most accurate transform, and the longest preprocessor execution time; greater values imply the opposite, but might lead to time or space savings in the transform itself, due to enhanced cachability.
*/
#define POPULATION_MIN 2
/*
Set PRECISION_IN, PRECISION_INTERNAL, and PRECISION_OUT as defined in number.h. Comparing results between various modes can provide some insights into the accuracy of the result, although thorough precision analysis is tricky. Note that (PRECISION_IN==2) holds only 80 bits, but must be padded with 16 high bits of 0; this is done to match GCC's expectations.
*/
#define PRECISION_IN 1
#define PRECISION_INTERNAL 2
#define PRECISION_OUT 1
