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
PRECISION_IN controls input file precision.
*/
#if PRECISION_IN==0
  #define NUMBER_IN float
/*
NUMBER_IN_U16_IDX_MAX is as defined in orthohack_file.h:orthohack_header_t:number_u16_idx_max, for input files, whether or not Orthohack files.
*/
  #define NUMBER_IN_U16_IDX_MAX 1
/*
NUMBER_IN_U16_PAD_HI_COUNT is the number of u16 0s after the sign bit of a NUMBER_IN in memory; it is not present in input files.
*/
  #define NUMBER_IN_U16_PAD_HI_COUNT 0
/*
NUMBER_IN_U16_PAD_LO_COUNT is the number of u16 0s before the least significant bit of a NUMBER_IN in memory; it is not present in input files.
*/
  #define NUMBER_IN_U16_PAD_LO_COUNT 0
#elif PRECISION_IN==1
  #define NUMBER_IN double
  #define NUMBER_IN_U16_IDX_MAX 3
  #define NUMBER_IN_U16_PAD_HI_COUNT 0
  #define NUMBER_IN_U16_PAD_LO_COUNT 0
#elif PRECISION_IN==2
  #define NUMBER_IN long double
  #define NUMBER_IN_U16_IDX_MAX 4
  #define NUMBER_IN_U16_PAD_HI_COUNT 1
  #define NUMBER_IN_U16_PAD_LO_COUNT 0
#else
  #error "PRECISION_IN set to invalid value."
#endif
/*
PRECISION_OUT controls output file precision.
*/
#if PRECISION_OUT==0
  #define NUMBER_OUT float
/*
NUMBER_OUT_U16_IDX_MAX is as defined in orthohack_file.h:orthohack_header_t:number_u16_idx_max, for output files, whether or not Orthohack files.
*/
  #define NUMBER_OUT_U16_IDX_MAX 1
/*
NUMBER_OUT_U16_PAD_HI_COUNT is the number of u16 0s after the sign bit of a NUMBER_OUT in memory; it is not present in output files.
*/
  #define NUMBER_OUT_U16_PAD_HI_COUNT 0
/*
NUMBER_OUT_U16_PAD_LO_COUNT is the number of u16 0s before the least significant bit of a NUMBER_OUT in memory; it is not present in output files.
*/
  #define NUMBER_OUT_U16_PAD_LO_COUNT 0
#elif PRECISION_OUT==1
  #define NUMBER_OUT double
  #define NUMBER_OUT_U16_IDX_MAX 3
  #define NUMBER_OUT_U16_PAD_HI_COUNT 0
  #define NUMBER_OUT_U16_PAD_LO_COUNT 0
#elif PRECISION_OUT==2
  #define NUMBER_OUT long double
  #define NUMBER_OUT_U16_IDX_MAX 4
  #define NUMBER_OUT_U16_PAD_HI_COUNT 1
  #define NUMBER_OUT_U16_PAD_LO_COUNT 0
#else
  #error "PRECISION_OUT set to invalid value."
#endif
