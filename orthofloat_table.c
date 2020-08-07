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
#define TABLE_FLOAT_DEFINED 1
/*
This is actually a table of floating-point values in some standard format, but we present it to the compiler as a byte (u8) list because don't want different platforms to compile different floating-point values, due to subtle differences in the conversion process from ASCII decimal to binary. If you try to port this code to another language, you'll have to do some manual work in order to export the data table as a bonafide floating-point list, without the conversion issue. But maybe it's all moot, because floating-point behavior is so ill-defined anyway. Yuck! Alternatively, see the ASCII table in ortholiteral_table.c, or use fixed-point or custom fixed-point if portability is an issue.

This readonly data structure is sufficiently overallocated to facilitate read-past-end for speed.
*/
const u8 number_list_base[NUMBER_LIST_SIZE+sizeof(NUMBER)]={NUMBER_LIST};
