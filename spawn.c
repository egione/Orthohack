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
#ifdef PTHREAD_PRESENT
#include <pthread.h>

#define SPAWN(thread_idx_max,thread_set_base) spawn_multi(thread_idx_max,thread_set_base)
#define SPAWN_FREE(thread_set_base) spawn_multi_free(thread_set_base)
#define SPAWN_INIT(function_base,readonly_string_base,simulthread_idx_max) spawn_multi_init(function_base,readonly_string_base,simulthread_idx_max)
#define SPAWN_ONE(thread_set_base,unique_idx) spawn_multi_one(thread_set_base,unique_idx)
#define SPAWN_RETIRE_ALL(thread_set_base) spawn_multi_retire_all(thread_set_base)
#define SPAWN_REWIND(function_base,readonly_string_base,thread_set_base) spawn_multi_rewind(function_base,readonly_string_base,thread_set_base)

void
spawn_multi_free(thread_set_t *thread_set_base){
  if(thread_set_base){
    string_free(thread_set_base->spawn_context_base);
    string_free(thread_set_base->thread_list_base);
    string_free(thread_set_base);
  }
  return;
}

void
spawn_multi_rewind(void (*function_base)(thread_context_t *),u8 *readonly_string_base,thread_set_t *thread_set_base){
/*
Reset the Spawn engine without changing the memory map.

In:

  function_base is as defined in spawn_multi_init():In.

  readonly_string_base is as defined in spawn_multi_init():In.

  *thread_set_base is as returned by spawn_multi_init().
*/
  u64 i;
  thread_t *thread_list_base;
  u64 simulthread_idx_max;

  string_zero(sizeof(spawn_context_t),(u8 *)(thread_set_base->spawn_context_base));
  thread_set_base->function_base=function_base;
  thread_list_base=thread_set_base->thread_list_base;
  simulthread_idx_max=thread_set_base->simulthread_idx_max;
  i=0;
  do{
    thread_list_base[i].thread_context.readonly_string_base=readonly_string_base;
  }while((i++)!=simulthread_idx_max);
  return;
}

thread_set_t *
spawn_multi_init(void (*function_base)(thread_context_t *),u8 *readonly_string_base,u64 simulthread_idx_max){
/*
Initialize the Spawn engine for multithreaded mode.

In:

  function_base is the base of a function which accepts a (thread_context_t *) and has no return value (because all communication occurs via memory). Typically, this is a bridge function which then invokes the "real" thread code.

  readonly_string_base is NULL, or the base of a string to which all threads shall be given read access, via thread_context_t.readonly_string_base.

  simulthread_idx_max is the maximum allowable number of simultaneous threads in flight, less 1. All values are valid. Useful to limit resource consumption and kernel overhead due to excessive simulthreads. The simulthread ID gets copied into the thread_context_t.simulthread_idx of each thread, which will never exceed thread_context_t.thread_idx. (In monothreaded mode, simulthread_idx is always 0.)

Out:

  Returns a (thread_set_t *) for use with future calls to the Spawn engine.
*/
  u64 i;
  spawn_context_t *spawn_context_base;
  thread_t *thread_list_base;
  u64 thread_list_size;
  thread_set_t *thread_set_base;

  thread_list_size=simulthread_idx_max;
  thread_list_size++;
  thread_list_size*=sizeof(thread_t);
  thread_list_base=(thread_t *)(string_create(thread_list_size));
  spawn_context_base=(spawn_context_t *)(string_create_zero(sizeof(spawn_context_t)));
  thread_set_base=NULL;
  if(thread_list_base&&spawn_context_base){
    thread_set_base=(thread_set_t *)(string_create(sizeof(thread_set_t)));
    if(thread_set_base){
      thread_set_base->thread_list_base=thread_list_base;
      thread_set_base->spawn_context_base=spawn_context_base;
      thread_set_base->function_base=function_base;
      thread_set_base->simulthread_idx_max=simulthread_idx_max;
      i=0;
      do{
        thread_list_base[i].thread_context.readonly_string_base=readonly_string_base;
        thread_list_base[i].thread_context.simulthread_idx=i;
      }while((i++)!=simulthread_idx_max);
    }
  }
  if(thread_set_base==NULL){
    string_free(spawn_context_base);
    string_free(thread_list_base);
  }
  return thread_set_base;
}

u8
spawn_multi_one(thread_set_t *thread_set_base,u64 unique_idx){
/*
Spawn a thread asynchronously.

In:

  unique_idx is a value used once until spawn_multi_retire_all() is called, after which it may be reused. It's used to address the reason for which this routine is called instead of spawn_multi(), namely, that the thread IDs aren't known ahead of time. For example, a master process could browse a list, looking for work to do. Then some of the items in the list would invoke slave threads, whereas others would not. In this example, a linearly increasing thread_idx would be of little use to the target function. unique_idx gets around this problem by permitting thread IDs to be sparse, and even out-of-order.

  *thread_set_base is as returned by spawn_multi_init().

Out:

  Returns 1 on failure, else 0. In the event that a fatal error occurs during either launch or retirement, this function will not return until all outstanding threads have been retired. Otherwise, memory could be freed which would still be in use by another thread.

  The caller must not call any other Spawn function except this one, until spawn_multi_retire_all() has been called.
*/
  void (*function_base)(thread_context_t *);
  int pthread_status;
  u8 retire_status;
  u64 simulthread_idx_max;
  u64 simulthread_launch_idx;
  u64 simulthread_retire_idx;
  spawn_context_t *spawn_context_base;
  u8 status;
  thread_t *thread_base;
  u64 thread_launch_idx;
  thread_t *thread_list_base;
  u64 thread_retire_idx;
  u64 thread_retire_idx_max;

  thread_list_base=thread_set_base->thread_list_base;
  spawn_context_base=thread_set_base->spawn_context_base;
  function_base=thread_set_base->function_base;
  simulthread_idx_max=thread_set_base->simulthread_idx_max;
  simulthread_launch_idx=spawn_context_base->simulthread_launch_idx;
  simulthread_retire_idx=spawn_context_base->simulthread_retire_idx;
  thread_launch_idx=spawn_context_base->thread_launch_idx;
  thread_retire_idx=spawn_context_base->thread_retire_idx;
  do{
    retire_status=1;
    thread_base=&thread_list_base[simulthread_launch_idx];
    thread_base->thread_context.thread_idx=unique_idx;
    do{
      pthread_status=pthread_create(&thread_base->pthread,NULL,(void *)(function_base),&thread_base->thread_context);
    }while((pthread_status==EAGAIN)&&(thread_launch_idx==thread_retire_idx));
    if(pthread_status==0){
      simulthread_launch_idx++;
      if(simulthread_launch_idx>simulthread_idx_max){
        simulthread_launch_idx=0;
      }
      if(simulthread_launch_idx==simulthread_retire_idx){
/*
We need to retire a thread because we're maxed out on resources.
*/
        thread_retire_idx_max=thread_retire_idx;
      }else{
        retire_status=0;
      }
      thread_launch_idx++;
      spawn_context_base->thread_launch_idx=thread_launch_idx;
    }else if((pthread_status==EAGAIN)||((pthread_status==ENOMEM)&&(thread_launch_idx!=thread_retire_idx))){
/*
We failed to launch the thread as requested by the caller, but we have at least 1 pending, and the OS is too busy to help us. Retire a thread and then try launching again.
*/
      pthread_status=EAGAIN;
      thread_retire_idx_max=thread_retire_idx;
    }else{
/*
There was a catastrophic error. Retire all pending threads.
*/
      if(thread_launch_idx!=thread_retire_idx){
        thread_retire_idx_max=thread_launch_idx-1;
      }else{
        retire_status=0;
      }
    }
    if(retire_status){
      do{
        thread_base=&thread_list_base[simulthread_retire_idx];
        do{
          status=!!pthread_join(thread_base->pthread,NULL);
/*
No matter what, do not exit this loop until the pthread_join has succeeded. If we hang, we hang. Better that, than corrupting memory and risking uncontrolled OS calls due to freeing memory in use by another thread.
*/
        }while(status);
        simulthread_retire_idx++;
        if(simulthread_retire_idx>simulthread_idx_max){
          simulthread_retire_idx=0;
        }
      }while((thread_retire_idx++)!=thread_retire_idx_max);
    }
/*
Keep trying to launch a thread until either we succeed, or we receive an unrecoverable error.
*/
  }while(pthread_status==EAGAIN);
  status=!!pthread_status;
  spawn_context_base->simulthread_launch_idx=simulthread_launch_idx;
  spawn_context_base->simulthread_retire_idx=simulthread_retire_idx;
  spawn_context_base->thread_launch_idx=thread_launch_idx;
  spawn_context_base->thread_retire_idx=thread_retire_idx;
  return status;
}

void
spawn_multi_retire_all(thread_set_t *thread_set_base){
/*
Retire all threads outstanding from spawn_multi_one().

In:

  *thread_set_base is as returned by spawn_multi_init().

Out:

  All pending threads, if any, have finished.
*/
  u64 simulthread_idx_max;  
  u64 simulthread_retire_idx;
  spawn_context_t *spawn_context_base;
  u8 status;
  thread_t *thread_base;
  u64 thread_launch_idx;
  thread_t *thread_list_base;
  u64 thread_retire_idx;

  thread_list_base=thread_set_base->thread_list_base;
  simulthread_idx_max=thread_set_base->simulthread_idx_max;
  spawn_context_base=thread_set_base->spawn_context_base;
  simulthread_retire_idx=spawn_context_base->simulthread_retire_idx;
  thread_launch_idx=spawn_context_base->thread_launch_idx;
  thread_retire_idx=spawn_context_base->thread_retire_idx;
  while(thread_retire_idx!=thread_launch_idx){
    thread_base=&thread_list_base[simulthread_retire_idx];
    do{
      status=!!pthread_join(thread_base->pthread,NULL);
/*
No matter what, do not exit this loop until the pthread_join has succeeded. If we hang, we hang. Better that, than corrupting memory and risking uncontrolled OS calls due to freeing memory in use by another thread.
*/
    }while(status);
    simulthread_retire_idx++;
    if(simulthread_retire_idx>simulthread_idx_max){
      simulthread_retire_idx=0;
    }
    thread_retire_idx++;
  }
  string_zero(sizeof(spawn_context_t),(u8 *)(thread_set_base->spawn_context_base));
  return;
}

u8
spawn_multi(u64 thread_idx_max,thread_set_t *thread_set_base){
/*
Keep the OS thread engine as busy as possible with pending threads, within the specified simultaneous thread limit. Make sure your threads are long enough that the typical launch latency (perhpas 1 ms) isn't significant.

In:

  This function must not be recursed or nested within spawn_multi_one(). Otherwise it's possible that OS thread handles will be exhausted.

  thread_idx_max is the maximum thread number.

  *thread_set_base is as returned by spawn_multi_init().

Out:

  Returns 1 on failure, else 0. Will not return until all launched threads have been retired, regardless of success or failure. Otherwise, memory could be freed which would still be in use by another thread.
*/
  void (*function_base)(thread_context_t *);
  int pthread_status;
  u64 simulthread_idx_max;
  u64 simulthread_launch_idx;
  u64 simulthread_retire_idx;
  u8 status;
  thread_t *thread_base;
  u64 thread_launch_idx;
  u64 thread_launch_idx_max;
  thread_t *thread_list_base;
  u64 thread_retire_idx;
  u64 thread_retire_idx_max;

  thread_list_base=thread_set_base->thread_list_base;
  function_base=thread_set_base->function_base;
  simulthread_idx_max=thread_set_base->simulthread_idx_max;
  simulthread_launch_idx=0;
  simulthread_retire_idx=0;
  thread_launch_idx=0;
  thread_retire_idx=0;
  do{
/*
Allow up to (simulthread_idx_max+1) simultaneous threads.
*/
    thread_launch_idx_max=thread_retire_idx+simulthread_idx_max;
    if((thread_launch_idx_max>thread_idx_max)||(thread_launch_idx_max<thread_retire_idx)){
      thread_launch_idx_max=thread_idx_max;
    }
    do{
      thread_base=&thread_list_base[simulthread_launch_idx];
      thread_base->thread_context.thread_idx=thread_launch_idx;
      do{
        pthread_status=pthread_create(&thread_base->pthread,NULL,(void *)(function_base),&thread_base->thread_context);
/*
Keep trying to launch a thread until either we succeed at putting at least 1 in flight (if that's not already the case), or we receive an unrecoverable error.
*/
      }while((pthread_status==EAGAIN)&&(thread_launch_idx==thread_retire_idx));
      if(pthread_status==0){
        simulthread_launch_idx++;
        if(simulthread_launch_idx>simulthread_idx_max){
          simulthread_launch_idx=0;
        }
      }
    }while((pthread_status==0)&&((thread_launch_idx++)!=thread_launch_idx_max));
/*
If we don't have at least 1 pending thread, then return an error.
*/
    if(thread_launch_idx!=thread_retire_idx){
/*
If we failed due to transient resource constraints, it's OK. We still have at least 1 pending thread.
*/
      if(pthread_status==EAGAIN){
        pthread_status=0;
      }
/*
If not all threads have yet been launched, and (pthread_status==0) then retire only 1 of them. Else retire all outstanding.
*/
      if((thread_launch_idx!=(thread_idx_max+1))&&(pthread_status==0)){
        thread_retire_idx_max=thread_retire_idx;
      }else{
        thread_retire_idx_max=thread_launch_idx-1;
      }
/*
Retire threads in FIFO order.
*/
      do{
        thread_base=&thread_list_base[simulthread_retire_idx];
        do{
          status=!!pthread_join(thread_base->pthread,NULL);
/*
No matter what, do not exit this loop until all the required (pthread_join)s have succeeded. If we hang, we hang. Better that, than corrupting memory and risking uncontrolled OS calls due to freeing memory in use by another thread.
*/
        }while(status);
        simulthread_retire_idx++;
        if(simulthread_retire_idx>simulthread_idx_max){
          simulthread_retire_idx=0;
        }
      }while((thread_retire_idx++)!=thread_retire_idx_max);
      status=!!pthread_status;
    }else{
      status=1;
    }
  }while((status==0)&&(thread_retire_idx!=(thread_idx_max+1)));
  return status;
}
#else
#define SPAWN(thread_idx_max,thread_set_base) spawn_mono(thread_idx_max,thread_set_base)
#define SPAWN_FREE(thread_set_base) spawn_mono_free(thread_set_base)
#define SPAWN_INIT(function_base,readonly_string_base,simulthread_idx_max) spawn_mono_init(function_base,readonly_string_base)
#define SPAWN_ONE(thread_set_base,unique_idx) spawn_mono_one(thread_set_base,unique_idx)
#define SPAWN_RETIRE_ALL(thread_set_base) spawn_mono_retire_all(thread_set_base)
#define SPAWN_REWIND(function_base,readonly_string_base,thread_set_base) spawn_mono_rewind(function_base,readonly_string_base,thread_set_base)

void
spawn_mono_free(thread_set_t *thread_set_base){
  if(thread_set_base){
    string_free(thread_set_base->thread_list_base);
    string_free(thread_set_base);
  }
  return;
}

void
spawn_mono_rewind(void (*function_base)(thread_context_t *),u8 *readonly_string_base,thread_set_t *thread_set_base){
/*
Reset the Spawn engine without changing the memory map.

In:

  function_base is as defined in spawn_mono_init():In.

  readonly_string_base is as defined in spawn_mono_init():In.

  *thread_set_base is as returned by spawn_mono_init().
*/
  thread_t *thread_list_base;

  string_zero(sizeof(spawn_context_t),(u8 *)(thread_set_base->spawn_context_base));
  thread_set_base->function_base=function_base;
  thread_list_base=thread_set_base->thread_list_base;
  thread_list_base->thread_context.readonly_string_base=readonly_string_base;
  return;
}

thread_set_t *
spawn_mono_init(void (*function_base)(thread_context_t *),u8 *readonly_string_base){
/*
Initialize the Spawn engine for monothreaded mode.

In:

  function_base is the base of a function which accepts a (thread_context_t *) and has no return value (because all communication occurs via memory). Typically, this is a bridge function which then invokes the "real" thread code.

  readonly_string_base is NULL, or the base of a string to which all threads shall be given read access, via thread_context_t.readonly_string_base.

Out:

  Returns a (thread_set_t *) for use with future calls to the Spawn engine.
*/
  spawn_context_t *spawn_context_base;
  thread_t *thread_list_base;
  thread_set_t *thread_set_base;

  thread_list_base=(thread_t *)(string_create(sizeof(thread_t)));
  spawn_context_base=(spawn_context_t *)(string_create_zero(sizeof(spawn_context_t)));
  thread_set_base=NULL;
  if(thread_list_base&&spawn_context_base){
    thread_set_base=(thread_set_t *)(string_create(sizeof(thread_set_t)));
    if(thread_set_base){
      thread_set_base->thread_list_base=thread_list_base;
      thread_set_base->function_base=function_base;
      thread_set_base->spawn_context_base=spawn_context_base;
      thread_set_base->simulthread_idx_max=0;
      thread_list_base->thread_context.readonly_string_base=readonly_string_base;
      thread_list_base->thread_context.simulthread_idx=0;
    }
  }
  if(thread_set_base==NULL){
    string_free(spawn_context_base);
    string_free(thread_list_base);
  }
  return thread_set_base;
}

u8
spawn_mono_one(thread_set_t *thread_set_base,u64 unique_idx){
/*
In:

  unique_idx is a value used once until spawn_mono_retire_all() is called, after which it may be reused. It's used to address the reason for which this routine is called instead of spawn_mono(), namely, that the thread IDs aren't known ahead of time. For example, a master process could browse a list, looking for work to do. Then some of the items in the list would invoke slave threads, whereas others would not. In this example, a linearly increasing thread_idx would be of little use to the target function. unique_idx gets around this problem by permitting thread IDs to be sparse, and even out-of-order.

  *thread_set_base is as returned by spawn_mono_init().

Out:

  Returns 0 for compatibility with spawn_multi_one().

  The caller must not call any other Spawn function except this one, until spawn_mono_retire_all() has been called.
*/
  void (*function_base)(thread_context_t *);
  spawn_context_t *spawn_context_base;
  u8 status;
  thread_t *thread_base;
  thread_context_t *thread_context_base;
  u64 thread_launch_idx;
  thread_t *thread_list_base;

  thread_list_base=thread_set_base->thread_list_base;
  spawn_context_base=thread_set_base->spawn_context_base;
  function_base=thread_set_base->function_base;
  thread_context_base=&thread_list_base->thread_context;
  thread_context_base->thread_idx=unique_idx;
  function_base(thread_context_base);
  thread_launch_idx=spawn_context_base->thread_launch_idx;
  thread_launch_idx++;
  spawn_context_base->thread_launch_idx=thread_launch_idx;
  return 0;
}

void
spawn_mono_retire_all(thread_set_t *thread_set_base){
/*
In:

  *thread_set_base is as returned by spawn_mono_init().

Out:

  Internal data structures are reinitialized consistent with spawn_multi_retire_all().
*/
  string_zero(sizeof(spawn_context_t),(u8 *)(thread_set_base->spawn_context_base));
  return;
}

u8
spawn_mono(u64 thread_idx_max,thread_set_t *thread_set_base){
/*
Monothreaded emulation of spawn_multi() for verification purposes or unicore environments.

In:

  thread_idx_max is the maximum thread number.

  *thread_set_base is as returned by spawn_mono_init().

Out:

  Returns 0 for compatibility with spawn_multi().
*/
  void (*function_base)(thread_context_t *);
  u64 i;
  thread_context_t *thread_context_base;
  thread_t *thread_list_base;

  thread_list_base=thread_set_base->thread_list_base;
  function_base=thread_set_base->function_base;
  thread_context_base=&thread_list_base->thread_context;
  i=0;
  do{
    thread_context_base->thread_idx=i;
    function_base((void *)(thread_context_base));
  }while((i++)!=thread_idx_max);
  return 0;
}
#endif

