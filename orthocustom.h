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
Change this file if you want to change anything regarding "custom-point" (fixed-point precision with a customizable fraction bit count) operations. You can add higher precision definitions by copying and  modifying the following macros.
*/
#define CUSTOM_DEFINED 1
/*
PRECISION_CUSTOM controls fixed-point calculation precision.
*/
#if PRECISION_CUSTOM<=1
  #define CUSTOM i32
/*
(CUSTOM)s operate just like signed integers, but with a number of fractional binary places to represent rational numbers between integers. CUSTOM_FRACTION_BIT_COUNT controls how many such bits exist, which must be less than the number of bits in the CUSTOM type. If you change it, you must recompile and rerun post, in order to generate a new orthohack.h. It should be on [1,30], but in theory other values may work.

If you're having trouble avoiding overflows, then perhaps you should just give up on custom fixed-point, and use fixed-point instead. To do that, comment out the line containing "OVERFLOW_CHECK" in config.h.
*/
  #define CUSTOM_FRACTION_BIT_COUNT 24
  #ifdef OVERFLOW_CHECK
    #define CUSTOM_FROM_NUMBER(n,c) \
      if(IS_GREATER(n,RATIO(NUMBER_MAKE(I32_MAX),NUMBER_MAKE(1<<CUSTOM_FRACTION_BIT_COUNT)))){ \
        printf("\nCUSTOM_FROM_NUMBER overflow; try decreasing CUSTOM_FRACTION_BIT_COUNT.\n"); \
        exit(0); \
      } \
      c=((i32)(PRODUCT(n,NUMBER_MAKE(1<<CUSTOM_FRACTION_BIT_COUNT))));
  #else
    #define CUSTOM_FROM_NUMBER(n,c) (c=((i32)(PRODUCT(n,NUMBER_MAKE(1<<CUSTOM_FRACTION_BIT_COUNT)))))
  #endif
  #define CUSTOM_IS_GREATER(a,b) ((a)>(b))
  #define CUSTOM_IS_LESS(a,b) ((a)<(b))

  #ifdef OVERFLOW_CHECK
    #define CUSTOM_NEGATIVE(a) \
      if((a)==I32_MIN){ \
        printf("\nCUSTOM_NEGATIVE overflow.\n"); \
        exit(0); \
      } \
      a=-(a);
  #else
    #define CUSTOM_NEGATIVE(a) (-(a))
  #endif

  #ifdef OVERFLOW_CHECK
    #if PRECISION_CUSTOM==0
      #define CUSTOM_PRODUCT(a,b,p) \
        do{ \
          i64 c; \
          \
          c=((i64)(a)*b)>>CUSTOM_FRACTION_BIT_COUNT; \
          p=(i32)(c); \
          c>>=U32_BIT_MAX; \
          if(c&&(c!=(-1))){ \
            printf("\nCUSTOM_PRODUCT overflow.\n"); \
            exit(0); \
          } \
        }while(0);
    #else
      #define CUSTOM_PRODUCT(a,b,p) \
        do{ \
          i64 c; \
          \
          c=(((i64)(a)*b)+(1<<(CUSTOM_FRACTION_BIT_COUNT-1)))>>CUSTOM_FRACTION_BIT_COUNT; \
          p=(i32)(c); \
          c>>=U32_BIT_MAX; \
          if(c&&(c!=(-1))){ \
            printf("\nCUSTOM_PRODUCT overflow.\n"); \
            exit(0); \
          } \
        }while(0);
    #endif
  #else
    #if PRECISION_CUSTOM==0
      #define CUSTOM_PRODUCT(a,b,p) (p=(i32)(((i64)(a)*b)>>CUSTOM_FRACTION_BIT_COUNT))
    #else
      #define CUSTOM_PRODUCT(a,b,p) (p=(i32)((((i64)(a)*b)+(1<<(CUSTOM_FRACTION_BIT_COUNT-1)))>>CUSTOM_FRACTION_BIT_COUNT))
    #endif
  #endif

  #define CUSTOM_SET_NEGATIVE_ONE(a) ((a)=(-1)<<CUSTOM_FRACTION_BIT_COUNT)
  #define CUSTOM_SET_ONE_MINUS_EPSILON(a) ((a)=((1<<CUSTOM_FRACTION_BIT_COUNT)-1))
  #define CUSTOM_SET_ZERO(a) ((a)=0)

  #ifdef OVERFLOW_CHECK
    #define CUSTOM_SIGN_SET(a,sign,b) \
      if(sign){ \
        if(a==(I32_MIN)){ \
          printf("\nCUSTOM_SIGN_SET overflow.\n"); \
          exit(0); \
        } \
        b=-(a); \
      }
  #else
    #define CUSTOM_SIGN_SET(a,sign,b) (b=((sign)?CUSTOM_NEGATIVE(a):(a)))
  #endif

  #ifdef OVERFLOW_CHECK
    #define CUSTOM_SUM(a,b,s) \
      do{ \
        i64 c; \
        \
        c=a; \
        c+=b; \
        s=(i32)(c); \
        c>>=U32_BIT_MAX; \
        if(c&&(c!=(-1))){ \
          printf("\nCUSTOM_SUM overflow.\n"); \
          exit(0); \
        } \
      }while(0);
  #else
    #define CUSTOM_SUM(a,b,s) (s=((a)+(b)))
  #endif

  #define CUSTOM_TO_NUMBER(a) (RATIO(NUMBER_MAKE(a),NUMBER_MAKE(1<<CUSTOM_FRACTION_BIT_COUNT)))
#else
  #error "PRECISION_CUSTOM set to invalid value."
#endif
