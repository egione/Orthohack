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
#define TABLE_DEFINED 1
/*
These readonly data structures are all sufficiently overallocated to facilitate read-past-end for speed.
*/
const u64 binomial_list_base[BINOMIAL_LIST_U64_COUNT+2]={BINOMIAL_LIST};
const u64 transform_string_base[TRANSFORM_STRING_U64_COUNT+2]={TRANSFORM_STRING};
