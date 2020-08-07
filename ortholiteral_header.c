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
Orthohack Literal Floating-Point Transform Header
*/
#ifndef CONSTANTS_DEFINED
#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include "orthoconstant.h"
#endif
/*
This filename must point to the captured output of post.c. It contains the NUMBER_LIST, BINOMIAL_LIST, and TRANSFORM_STRING necessary to perform the transform.
*/
#ifndef ORTHOHACK_DEFINED
#include "orthohack.h"
#endif
#ifndef NUMBER_COMPONENT_IDX_MAX
#include "orthodim.h"
#endif
#ifndef NUMBER_DEFINED
#include "orthonumber.h"
#endif
#ifndef TABLE_LITERAL_DEFINED
#include "ortholiteral_table.c"
#endif
/*
The following definitions control the fundamental operations in the generated ortholiteral.c. Names are kept short in order to prevent compile time from exploding. If you look at orthofloat_transform(), from which ortholiteral_transform() was derived, you can make the following associations:

  l is literal_list_base (equivalent to number_list_base in orthofloat_transform()).
  m is monomial_list_base.
  c is coeff_list_base.
  v is ratio_idx.
  w is numerator_idx.
  x is denominator_idx.
  y is monomial_idx.
  z is coeff_idx.
*/
#define A(v,w,x,y) (m[y]=SUM(m[x],PRODUCT(l[v],m[w])))
#define B(v,w,x,y) (m[y]=DIFFERENCE(m[x],PRODUCT(l[v],m[w])))
#define C(v,y,z) (c[z]=SUM(c[z],PRODUCT(l[v],m[y])))
#define D(v,y,z) (c[z]=DIFFERENCE(c[z],PRODUCT(l[v],m[y])))

void
ortholiteral_init(NUMBER monomial_list_base[MONOMIAL_COUNT]){
/*
Prepare for a floating-point transform.

In:

  *monomial_list_base is writable for MONOMIAL_COUNT (NUMBER)s.

Out:

  *monomial_list_base is formatted so as to satisfy the assumptions of a floating-point transform.
*/
  NUMBER number;

  SET_ZERO(number);
  *monomial_list_base=number;
  return;
}

void
ortholiteral_transform(NUMBER c[COEFF_COUNT],NUMBER m[MONOMIAL_COUNT]){
/*
Perform the floating-point transform optimized by pre and post at the precision defined by PRECISION_INTERNAL in orthohack.h, which in turn results in the definition of a NUMBER by number.h.

NOTE: Variable names are abbreviated in order to cut down on compile time for the generated ortholiteral.c, which might be huge.

In:

  *c(oeff_list_base) is undefined and writable for COEFF_COUNT (NUMBER)s.

  *m(onomial_list_base) was initialized with ortholiteral_init(). SAMPLE_IDX_MAX (NUMBER)s starting at index 1 contain the spatial samples to transform, in the same order as in the orthobasis files which were originally fed into pre. Otherwise undefined and writable for MONOMIAL_COUNT (NUMBER)s.

Out:

  *c(oeff_list_base) contains the coefficients, expressed as (NUMBER)s, and arranged in the order corresponding to the ascending filenames which were originally fed into pre.

  *m(onomial_list_base) is unchanged where it was defined on input (the first (SAMPLE_IDX_MAX+1) (NUMBER)s). Otherwise undefined.
*/
