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
You don't need anything in unix_include.h. We just need it for rendering the demo output here -- not for doing the actual transforms.
*/
#include "../unix_include.h"
/*
You don't need build.h, but I do, just in order to get BUILD_NUMBER.
*/
#include "../build.h"
/*
In your own code, you generally only need a subset of these "ortho" includes, depending on whether you wish to use floating-point (orthonumber.h, orthofloat.c, orthofloat_x8.c), literal floating-point (ortholiteral_header.c, some_prefix_ortholiteral.c, ortholiteral_footer.c), fixed-point (orthofixed.h, orthofixed.c, orthofixed_x8.c), or custom fixed-point (orthocustom.h, orthocustom.c, orthocustom_x8.c) math, and whether you wish to do it in X1 or faster X8 topology (or both, for doing many transforms which are not multiples of 8). And certainly some variety of orthohack.h, such as the one included below, is necessary because it contains all the data tables that you'll need to do the transform. (Isn't it cool how changing a single include file, based on a build switch, can toggle between forward and inverse transforms? If you want to use the same technique, then just make sure that use the same orthonumber.h, orthofixed.h, and orthocustom.h in order to generate each orthohack.h, unless you want to create forks thereof for the purpose of using different numerical precision.) By the way, "orthonumber.h" is required for the floating point transforms. (Sorry, we use "float" to refer to stuff that actually uses (NUMBER)s. In hindsight, this was a poor choice of terminology.) orthoconstant.h is universally required.
*/
#include "../orthoconstant.h"
#ifdef TRANSFORM_FORWARD
  #include "dct_8x8_orthohack.h"
#elif defined(TRANSFORM_INVERSE)
  #include "idct_8x8_orthohack.h"
#else
  #error "You must define either TRANSFORM_FORWARD or TRANSFORM_INVERSE using the -D command line switch for gcc."
#endif
#include "../orthodim.h"
#include "../orthonumber.h"
#include "../orthofixed.h"
#include "../orthocustom.h"
#include "../ortholiteral_header.c"
#ifdef TRANSFORM_FORWARD
  #include "dct_8x8_ortholiteral.c"
#else
  #include "idct_8x8_ortholiteral.c"
#endif
#include "../ortholiteral_footer.c"
#include "../orthofloat.c"
#include "../orthofloat_x8.c"
#include "../orthofixed.c"
#include "../orthofixed_x8.c"
#include "../orthocustom.c"
#include "../orthocustom_x8.c"
/*
You don't need any includes below this line. They just help to produce the demo output. You can use malloc(), memset(), and free() instead of the fancier string.c functions, and you don't need anything in debug.c, which is just handy for debugging purposes herein. And of course, you don't need to print anything in order to do math! Orthohack output functions require no OS calls or mysterious third-party binaries in order to perform precomputed transforms, and never will.
*/
#include "../debug.c"
#include "../string.c"
#include "../print.c"

void
print_8x8(u8 *title_string_base,NUMBER *matrix_base){
  u32 x;
  u32 y;
/*
Print an 8x8 matrix in the same awkward format as GNU Octave.
*/
  printf("%s:\n\n Columns 1 through 3: \n\n",title_string_base);
  for(x=0;x<=7;x++){
    for(y=0;y<=2;y++){
      if(IS_NONNEGATIVE(matrix_base[(y*8)+x])){
        printf(" ",matrix_base[(y*8)+x]);
      }
      printf("  %1.14E",matrix_base[(y*8)+x]);
    }
    print_new_line();
  }
  printf("\n Columns 4 through 6: \n\n");
  for(x=0;x<=7;x++){
    for(y=3;y<=5;y++){
      if(IS_NONNEGATIVE(matrix_base[(y*8)+x])){
        printf(" ",matrix_base[(y*8)+x]);
      }
      printf("  %1.14E",matrix_base[(y*8)+x]);
    }
    print_new_line();
  }
  printf("\n Columns 7 and 8: \n\n");
  for(x=0;x<=7;x++){
    for(y=6;y<=7;y++){
      if(IS_NONNEGATIVE(matrix_base[(y*8)+x])){
        printf(" ",matrix_base[(y*8)+x]);
      }
      printf("  %1.14E",matrix_base[(y*8)+x]);
    }
    print_new_line();
  }
  return;
}

void
print_dct_idct_8x8(u8 *title_string_base,NUMBER *matrix_base){
#ifdef TRANSFORM_FORWARD
  printf("\nDCT(X) from ");
#else
  printf("\nIDCT(X) from ");
#endif
  print_8x8(title_string_base,matrix_base);
  return;
}

int
main(int argc, char *argv[]){
  CUSTOM *coeff_custom_list_base;
  CUSTOM *coeff_custom_list_x8_base;
  CUSTOM *coeff_custom_x8_list_base;
  FIXED *coeff_fixed_list_base;
  FIXED *coeff_fixed_list_x8_base;
  FIXED *coeff_fixed_x8_list_base;
  NUMBER *coeff_float_list_base;
  NUMBER *coeff_float_list_x8_base;
  NUMBER *coeff_float_x8_list_base;
  u32 idx;
  CUSTOM *monomial_custom_list_base;
  CUSTOM *monomial_custom_x8_list_base;
  FIXED *monomial_fixed_list_base;
  FIXED *monomial_fixed_x8_list_base;
  NUMBER *monomial_float_list_base;
  NUMBER *monomial_float_x8_list_base;
  u8 thread_idx;
  CUSTOM x_custom_list_x8_base[SAMPLE_IDX_MAX*8];
  FIXED x_fixed_list_x8_base[SAMPLE_IDX_MAX*8];
  NUMBER x_float_list_base[SAMPLE_IDX_MAX];
  NUMBER x_float_list_x8_base[SAMPLE_IDX_MAX*8];
  NUMBER y_float_list_base[SAMPLE_IDX_MAX];
  u8 status;

#ifdef TRANSFORM_FORWARD
  print_intro("8x8 DCT Demo");
#elif defined(TRANSFORM_INVERSE)
  print_intro("8x8 IDCT Demo");
#endif
/*
Emulate the code in dct_8x8_demo.m, which initializes a test matrix (x_float_list_base) with weird sample values. Note that SAMPLE_IDX_MAX is actually 0x40, meaning that there are 0x41 samples at the base of the monomial list (or 0x41*8 for X8 transforms), on account of the first sample always being 0. However, x_float_list_base begins with an actual sample point, which only happens to be 0.0 in this example code, but is not required to be. Thus SAMPLE_IDX_MAX is actually the number of items at x_float_list_base -- not its maximum index. x_float_list_base won't be directly used below; instead, we'll just use it as a database from which to populate items 1 through 0x40 of the applicable monomial list.
*/
  string_zero(SAMPLE_IDX_MAX*sizeof(NUMBER),(u8 *)(x_float_list_base));
  for(idx=0;idx<SAMPLE_IDX_MAX;idx++){
    if((idx%3)==0){
      x_float_list_base[idx]=RATIO(NUMBER_MAKE(idx),NUMBER_MAKE(97));
    }else if((idx%3)==1){
      x_float_list_base[idx]=NEGATIVE(RATIO(NUMBER_MAKE(idx),NUMBER_MAKE(137)));
    }else{
      x_float_list_base[idx]=NEGATIVE(RATIO(NUMBER_MAKE(idx),NUMBER_MAKE(71)));
    }
/*
For the X8 transforms, we'll need 8 copies of x_float_list_base at x_float_list_x8_base. (Normally we would use 8 different sample sets, providing for 8 parallel (or at least tightly pipelined) transforms, but here we just duplicate 1 set 8 times for demo purposes.) While we're at it, convert to fixed-point and custom fixed-point as well.
*/
    for(thread_idx=0;thread_idx<=7;thread_idx++){
      CUSTOM_FROM_NUMBER(x_float_list_base[idx],x_custom_list_x8_base[idx+(thread_idx*SAMPLE_IDX_MAX)]);
      x_fixed_list_x8_base[idx+(thread_idx*SAMPLE_IDX_MAX)]=FIXED_FROM_NUMBER(x_float_list_base[idx]);
      x_float_list_x8_base[idx+(thread_idx*SAMPLE_IDX_MAX)]=x_float_list_base[idx];
    }
  }
  printf("Compare these values to matlab/dct_idct_8x8_demo_output.txt:\n");
  print_8x8("\nInput matrix X",x_float_list_base);
/*
START 8x8 DCT, literal floating-point.
*/
  coeff_float_list_base=(NUMBER *)(string_create(COEFF_COUNT*sizeof(NUMBER)));
  status=!coeff_float_list_base;
  monomial_float_list_base=(NUMBER *)(string_create(MONOMIAL_COUNT*sizeof(NUMBER)));
  status|=!monomial_float_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  ortholiteral_init(monomial_float_list_base);
/*
Copy x_float_list_base to the monomial list, starting at index 1. (Normally, this copy step would be eliminated because you would simply start the world with the samples being read directly into the monomial list, which is why ortholiteral_init() doesn't do the copy for you.) We can leave everything else uninitialized.
*/
  for(idx=0;idx<SAMPLE_IDX_MAX;idx++){
    monomial_float_list_base[idx+1]=x_float_list_base[idx];
  }
  ortholiteral_transform(coeff_float_list_base,monomial_float_list_base);
  print_dct_idct_8x8("literal floating-point",coeff_float_list_base);
  string_free(monomial_float_list_base);
  string_free(coeff_float_list_base);
/*
END 8x8 DCT, literal floating-point.

START 8x8 DCT, floating-point X1. Proceed in the fashion of literal floating-point.
*/
  coeff_float_list_base=(NUMBER *)(string_create(COEFF_COUNT*sizeof(NUMBER)));
  status=!coeff_float_list_base;
  monomial_float_list_base=(NUMBER *)(string_create(MONOMIAL_COUNT*sizeof(NUMBER)));
  status|=!monomial_float_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  orthofloat_init(monomial_float_list_base);
/*
Copy x_float_list_base to the monomial list, starting at index 1. (Normally, this copy step would be eliminated because you would simply start the world with the samples being read directly into the monomial list, which is why orthofloat_init doesn't do the copy for you.) We can leave everything else uninitialized.
*/
  for(idx=0;idx<SAMPLE_IDX_MAX;idx++){
    monomial_float_list_base[idx+1]=x_float_list_base[idx];
  }
  orthofloat_transform(coeff_float_list_base,monomial_float_list_base);
  print_dct_idct_8x8("floating-point X1",coeff_float_list_base);
  string_free(monomial_float_list_base);
  string_free(coeff_float_list_base);
/*
END 8x8 DCT, floating-point X1.

START 8x8 DCT, floating-point X8.
*/
  coeff_float_list_x8_base=(NUMBER *)(string_create(COEFF_COUNT*8*sizeof(NUMBER)));
  status=!coeff_float_list_x8_base;
  coeff_float_x8_list_base=(NUMBER *)(string_create(COEFF_COUNT*8*sizeof(NUMBER)));
  status=!coeff_float_x8_list_base;
  monomial_float_x8_list_base=(NUMBER *)(string_create(MONOMIAL_COUNT*8*sizeof(NUMBER)));
  status|=!monomial_float_x8_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
/*
orthofloat_x8_init() does the X8 thread splicing for us, from x_float_list_x8_base to monomial_x8_list_base (note "list_x8" vs. "x8_list") because the source data is usually formatted as a concatenation of complete threads (as is the case of *x_float_list_x8_base), as opposed to a X8 interleave thereof.
*/
  orthofloat_x8_init(monomial_float_x8_list_base,x_float_list_x8_base);
  orthofloat_x8_transform(coeff_float_x8_list_base,monomial_float_x8_list_base);
/*
Unsplice the coefficient threads, from *coeff_float_x8_list_base to *coeff_float_list_x8_base.
*/
  orthofloat_x8_end(coeff_float_list_x8_base,coeff_float_x8_list_base);
/*
Take the results from thread 7 because it's the most susceptible to memory corruption if we did something wrong. Any thread should work just as well, as we set them to all the same input matrices. Technically, the threads could vary slightly in their outputs, since floating-point is ill-defined in C and may suffer from optimization differences from line to line.
*/
  print_dct_idct_8x8("floating-point X8",&coeff_float_list_x8_base[COEFF_COUNT*7]);
  string_free(monomial_float_x8_list_base);
  string_free(coeff_float_x8_list_base);
  string_free(coeff_float_list_x8_base);
/*
END 8x8 DCT, floating-point X8.

START 8x8 DCT, fixed-point X1. Proceed in the fashion of floating-point X1.
*/
  coeff_fixed_list_base=(FIXED *)(string_create(COEFF_COUNT*sizeof(FIXED)));
  status=!coeff_fixed_list_base;
  monomial_fixed_list_base=(FIXED *)(string_create(MONOMIAL_COUNT*sizeof(FIXED)));
  status|=!monomial_fixed_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  orthofixed_init(monomial_fixed_list_base);
  for(idx=0;idx<SAMPLE_IDX_MAX;idx++){
    monomial_fixed_list_base[idx+1]=FIXED_FROM_NUMBER(x_float_list_base[idx]);
  }
  orthofixed_transform(coeff_fixed_list_base,monomial_fixed_list_base);
/*
Convert the output to floating-point for display.
*/
  for(idx=0;idx<COEFF_COUNT;idx++){
    y_float_list_base[idx]=FIXED_TO_NUMBER(coeff_fixed_list_base[idx]);
  }
  print_dct_idct_8x8("fixed-point X1",y_float_list_base);
  string_free(monomial_fixed_list_base);
  string_free(coeff_fixed_list_base);
/*
END 8x8 DCT, fixed-point X1.

START 8x8 DCT, fixed-point X8. Proceed in the fashion of floating-point X8.
*/
  coeff_fixed_list_x8_base=(FIXED *)(string_create(COEFF_COUNT*8*sizeof(FIXED)));
  status=!coeff_fixed_list_x8_base;
  coeff_fixed_x8_list_base=(FIXED *)(string_create(COEFF_COUNT*8*sizeof(FIXED)));
  status=!coeff_fixed_x8_list_base;
  monomial_fixed_x8_list_base=(FIXED *)(string_create(MONOMIAL_COUNT*8*sizeof(FIXED)));
  status|=!monomial_fixed_x8_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  orthofixed_x8_init(monomial_fixed_x8_list_base,x_fixed_list_x8_base);
  orthofixed_x8_transform(coeff_fixed_x8_list_base,monomial_fixed_x8_list_base);
  orthofixed_x8_end(coeff_fixed_list_x8_base,coeff_fixed_x8_list_base);
/*
Convert the output to floating-point for display. Use thread 7 because it's most susceptible to memory corruption if we did something wrong.
*/
  for(idx=0;idx<COEFF_COUNT;idx++){
    y_float_list_base[idx]=FIXED_TO_NUMBER(coeff_fixed_list_x8_base[idx+(COEFF_COUNT*7)]);
  }
  print_dct_idct_8x8("fixed-point X8",y_float_list_base);
  string_free(monomial_fixed_x8_list_base);
  string_free(coeff_fixed_x8_list_base);
  string_free(coeff_fixed_list_x8_base);
/*
END 8x8 DCT, fixed-point X8.

START 8x8 DCT, custom fixed-point X1. Proceed in the fashion of fixed-point X1.
*/
  coeff_custom_list_base=(CUSTOM *)(string_create(COEFF_COUNT*sizeof(CUSTOM)));
  status=!coeff_custom_list_base;
  monomial_custom_list_base=(CUSTOM *)(string_create(MONOMIAL_COUNT*sizeof(CUSTOM)));
  status|=!monomial_custom_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  orthocustom_init(monomial_custom_list_base);
  for(idx=0;idx<SAMPLE_IDX_MAX;idx++){
    CUSTOM_FROM_NUMBER(x_float_list_base[idx],monomial_custom_list_base[idx+1]);
  }
  orthocustom_transform(coeff_custom_list_base,monomial_custom_list_base);
  for(idx=0;idx<COEFF_COUNT;idx++){
    y_float_list_base[idx]=CUSTOM_TO_NUMBER(coeff_custom_list_base[idx]);
  }
  print_dct_idct_8x8("custom fixed-point X1",y_float_list_base);
  string_free(monomial_custom_list_base);
  string_free(coeff_custom_list_base);
/*
END 8x8 DCT, custom fixed-point X1.

START 8x8 DCT, custom fixed-point X8. Proceed in the fashion of fixed-point X8.
*/
  coeff_custom_list_x8_base=(CUSTOM *)(string_create(COEFF_COUNT*8*sizeof(CUSTOM)));
  status=!coeff_custom_list_x8_base;
  coeff_custom_x8_list_base=(CUSTOM *)(string_create(COEFF_COUNT*8*sizeof(CUSTOM)));
  status=!coeff_custom_x8_list_base;
  monomial_custom_x8_list_base=(CUSTOM *)(string_create(MONOMIAL_COUNT*8*sizeof(CUSTOM)));
  status|=!monomial_custom_x8_list_base;
  if(status){
    print_error_exit_out_of_memory();
  }
  orthocustom_x8_init(monomial_custom_x8_list_base,x_custom_list_x8_base);
  orthocustom_x8_transform(coeff_custom_x8_list_base,monomial_custom_x8_list_base);
  orthocustom_x8_end(coeff_custom_list_x8_base,coeff_custom_x8_list_base);
  for(idx=0;idx<COEFF_COUNT;idx++){
    y_float_list_base[idx]=CUSTOM_TO_NUMBER(coeff_custom_list_x8_base[idx+(COEFF_COUNT*7)]);
  }
  print_dct_idct_8x8("custom-point X8",y_float_list_base);
  string_free(monomial_custom_x8_list_base);
  string_free(coeff_custom_x8_list_base);
  string_free(coeff_custom_list_x8_base);
/*
END 8x8 DCT, custom fixed-point X8.
*/
  return 0;
}
