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
Set PRECISION_IN and PRECISION_INTERNAL as defined in number.h. PRECISION_OUT will control the precision at which the resulting floating-point transform is conducted; and similarly with PRECISION_FIXED for fixed-point and PRECISION_CUSTOM for custom fixed-point (0 for fast, 1 for slightly slower and more accurate). Comparing results between various modes can provide some insights into the accuracy of the result, although thorough precision analysis is tricky. Note that (PRECISION_IN==2) holds only 80 bits, but must be padded with 16 high bits of 0; this is done to match GCC's expectations.
*/
#define PRECISION_CUSTOM 1
#define PRECISION_FIXED 0
#define PRECISION_IN 1
#define PRECISION_INTERNAL 2
#define PRECISION_OUT 1
