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
Change this file if you want to change anything regarding numerical precision. You can add higher precision definitions by copying and trivially modifying the following macros.
*/
#define NUMBER_DEFINED 1
/*
PRECISION_INTERNAL controls internal calculation precision.
*/
#if PRECISION_INTERNAL==0
  #define DEBUG_NUMBER debug_float
  #define DEBUG_NUMBER_LIST debug_float_list_print
/*
For complexes and quaternions, MAGNITUDE() returns the square root of the sum of the squares of all the number components.
*/
  #define MAGNITUDE(a) fabsf(a)
  #define NUMBER float
/*
For complexes and quaternions, SIGN_ZERO() makes the signs of all the number components positive.
*/
  #define SIGN_ZERO(a) fabsf(a)
#elif PRECISION_INTERNAL==1
  #define DEBUG_NUMBER debug_double
  #define DEBUG_NUMBER_LIST debug_double_list_print
  #define MAGNITUDE(a) fabs(a)
  #define NUMBER double
  #define SIGN_ZERO(a) fabs(a)
#elif PRECISION_INTERNAL==2
  #define DEBUG_NUMBER debug_long_double
  #define DEBUG_NUMBER_LIST debug_long_double_list_print
  #define MAGNITUDE(a) fabsl(a)
  #define NUMBER long double
  #define SIGN_ZERO(a) fabsl(a)
#else
  #error "PRECISION_INTERNAL set to invalid value."
#endif
/*
The following definitions are probably unnecessary for scalar real floating-point values, but will help in the future if we want to extend Orthohack to custom-precision floating-point, complex, or quaternion numbers. Complex and other multipartite numbers would be a bit difficult, because they don't have the ordering properties that allow us to sort real numbers and look them up quickly, but with the addition of a few functions, they could be processed correctly. NOTE: Keep the parentheses around all expressions, lest the compiler evaluate them in the wrong way!
*/
#define DIFFERENCE(a,b) ((a)-(b))
#define DISTANCE_FROM_ONE(a) ((a)-NUMBER_MAKE(1.0f))
#define IS_EQUAL(a,b) ((a)==(b))
#define IS_GREATER(a,b) ((a)>(b))
#define IS_GREATER_EQUAL(a,b) ((a)>=(b))
#define IS_LESS(a,b) ((a)<(b))
#define IS_LESS_EQUAL(a,b) ((a)<=(b))
#define IS_NEGATIVE(a) ((a)<NUMBER_MAKE(0.0f))
#define IS_NONNEGATIVE(a) ((a)>=NUMBER_MAKE(0.0f))
#define IS_NONZERO(a) ((a)!=NUMBER_MAKE(0.0f))
#define IS_UNEQUAL(a,b) ((a)!=(b))
#define IS_ZERO(a) ((a)==NUMBER_MAKE(0.0f))
#define NEGATIVE(a) (-(a))
#define NUMBER_MAKE(a) ((NUMBER)(a))
#define NUMBER_OUT_MAKE(a) ((NUMBER_OUT)(a))
#define PRODUCT(a,b) ((a)*(b))
#define RATIO(a,b) ((a)/(b))
#define SET_NEGATIVE_ONE(a) ((a)=NUMBER_MAKE(-1.0f))
#define SET_ONE(a) ((a)=NUMBER_MAKE(1.0f))
#define SET_ZERO(a) ((a)=NUMBER_MAKE(0.0f))
/*
For complexes and quaternions, SIGN_GET() returns a bitstring containing (NUMBER_COMPONENT_IDX_MAX+1) bits, with the sign of the real component at the LSB.
*/
#define SIGN_GET(a) ((a)<NUMBER_MAKE(0.0f))
/*
SIGN_SET() must do the opposite of SIGN_GET(), which is potentially nontrivial with multipartite numbers.
*/
#define SIGN_SET(a,sign) ((sign)?NEGATIVE(a):(a))
#define SUM(a,b) ((a)+(b))
/*
You don't need to change anything beyond this line.
*/
#define NUMBER_IN_PAD(number_in_base,number_in) memcpy((void *)((u8 *)(&(number_in))+NUMBER_IN_U16_PAD_LO_COUNT),(const void *)(number_in_base),sizeof(NUMBER_IN)-NUMBER_IN_U16_PAD_LO_COUNT-NUMBER_IN_U16_PAD_HI_COUNT)
#define NUMBER_OUT_UNPAD(number_out,number_out_base) memcpy((void *)(number_out_base),(const void *)((u8 *)(&(number_out))+NUMBER_OUT_U16_PAD_LO_COUNT),sizeof(NUMBER_OUT)-NUMBER_OUT_U16_PAD_LO_COUNT-NUMBER_OUT_U16_PAD_HI_COUNT)
