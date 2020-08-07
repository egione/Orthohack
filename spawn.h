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
TYPEDEF_START
  u64 simulthread_launch_idx;
  u64 simulthread_retire_idx;
  u64 thread_launch_idx;
  u64 thread_retire_idx;
TYPEDEF_END(spawn_context_t)

TYPEDEF_START
  u8 *readonly_string_base;
  u64 simulthread_idx;
  u64 thread_idx;
TYPEDEF_END(thread_context_t)

TYPEDEF_START
  thread_context_t thread_context;
#ifdef PTHREAD_PRESENT
  pthread_t pthread;
#endif
TYPEDEF_END(thread_t)

TYPEDEF_START
  thread_t *thread_list_base;
  spawn_context_t *spawn_context_base;
  void (*function_base)(thread_context_t *);
  u64 simulthread_idx_max;
TYPEDEF_END(thread_set_t)

