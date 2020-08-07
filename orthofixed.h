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
Change this file if you want to change anything regarding fixed-point precision. You can add higher precision definitions by copying and  modifying the following macros.
*/
#define FIX_DEFINED 1
/*
PRECISION_FIXED controls fixed-point calculation precision.
*/
#if PRECISION_FIXED==0
  #define FIXED i64
  #define FIXED_FROM_NUMBER(a) ((i64)(PRODUCT(a,NUMBER_MAKE(4294967296.0f))))
  #define FIXED_IS_GREATER(a,b) ((a)>(b))
  #define FIXED_IS_LESS(a,b) ((a)<(b))
  #define FIXED_NEGATIVE(a) (-(a))

  #define FIXED_PRODUCT(a,b,product) \
    do{ \
      i32 a_hi; \
      u32 a_lo; \
      i32 b_hi; \
      u32 b_lo; \
      u64 product_lo; \
      i64 product_mid0; \
      i64 product_mid1; \
      i64 product_hi; \
      \
      a_lo=(u32)(a); \
      b_lo=(u32)(b); \
      a_hi=(i32)((a)>>U32_BITS); \
      b_hi=(i32)((b)>>U32_BITS); \
      product_lo=(u64)(a_lo)*b_lo; \
      product_hi=a_hi*b_hi; \
      product_mid0=a_lo*(i64)(b_hi); \
      product_mid1=(i64)(a_hi)*b_lo; \
      product_lo>>=U32_BITS; \
      product_hi<<=U32_BITS; \
      product=product_mid0+product_mid1+product_lo+product_hi; \
    }while(0)

  #define FIXED_SET_NEGATIVE_ONE(a) ((a)=-1LL-U32_MAX)
  #define FIXED_SET_ONE_MINUS_EPSILON(a) ((a)=U32_MAX)
  #define FIXED_SET_ZERO(a) ((a)=0)
  #define FIXED_SIGN_SET(a,sign) ((sign)?FIXED_NEGATIVE(a):(a))
  #define FIXED_SUM(a,b) ((a)+(b))
  #define FIXED_TO_NUMBER(a) (RATIO(NUMBER_MAKE(a),NUMBER_MAKE(4294967296.0f)))
#else
  #error "PRECISION_FIXED set to invalid value."
#endif
