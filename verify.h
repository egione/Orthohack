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
Set PRECISION_IN and PRECISION_INTERNAL as defined in number.h. PRECISION_IN should match the precision of the original orthobasis files, which will be used for transform verification. PRECISION_INTERNAL merely affects the variables used for the purpose of verification. (There is no PRECISION_OUT because verification does not require an output file. The precision of the transforms to be verified are actually controlled by PRECISION_OUT and PRECISION_FIXED in post.h.) Comparing results between various modes can provide some insights into the accuracy of the result, although thorough precision analysis is tricky. Note that (PRECISION_IN==2) holds only 80 bits, but must be padded with 16 high bits of 0; this is done to match GCC's expectations.
*/
#define PRECISION_IN 1
#define PRECISION_INTERNAL 2
