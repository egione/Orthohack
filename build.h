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
Build control. If possible, change the build using gcc command switches, and not by changing this file.
*/
#define ORTHOHACK_BUILD_ID 94

#if !(defined(_32_)||defined(_64_))
#error "Use \"gcc -D_64_\" for 64-bit or \"gcc -D_32_\" for 32-bit code."
#elif defined(_32_)&&defined(_64_)
#error "You have defined both _32_ and _64_. Chose one only."
#endif

#if !(defined(DEBUG)||defined(NODEBUG))
#error "Use \"gcc -DDEBUG\" for debugging or \"gcc -DNODEBUG\" for release."
#elif defined(DEBUG)&&defined(NODEBUG)
#error "You have defined both DEBUG and NODEBUG. Chose one only."
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include <pthread.h>
#define PTHREAD_PRESENT
