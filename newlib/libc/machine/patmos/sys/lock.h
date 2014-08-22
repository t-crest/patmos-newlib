// Copyright 2014  Technical University of Denmark, DTU Compute.
//
// This file is part of the newlib C library for the Patmos processor.
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// (COPYING3.LIB). If not, see <http://www.gnu.org/licenses/>.

#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#ifndef __rtems__

#ifndef MAX_CORES
#define MAX_CORES 64
#endif

/* Locking structure to match Lamport's bakery locks */
typedef struct {
  volatile unsigned char entering [MAX_CORES];
  volatile unsigned int number [MAX_CORES];
} _LOCK_T;

typedef struct {
  _LOCK_T lock;
  volatile unsigned char owner;
  volatile unsigned int depth;
} _LOCK_RECURSIVE_T;
 

#define __EMPTY_LOCK { { 0 }, { 0 } }

#define __LOCK_INIT(class,lock) \
  class _LOCK_T lock = __EMPTY_LOCK;
#define __LOCK_INIT_RECURSIVE(class,lock) \
  class _LOCK_RECURSIVE_T lock = { __EMPTY_LOCK, -1, 0 };


#define __lock_init(lock)                  __patmos_lock_init(&(lock))
#define __lock_init_recursive(lock)        __patmos_lock_init_recursive(&(lock))
#define __lock_close(lock)                 __patmos_lock_close(&(lock))
#define __lock_close_recursive(lock)       __patmos_lock_close_recursive(&(lock))
#define __lock_acquire(lock)               __patmos_lock_acquire(&(lock))
#define __lock_acquire_recursive(lock)     __patmos_lock_acquire_recursive(&(lock))
#define __lock_try_acquire(lock)           __patmos_lock_try_acquire(&(lock))
#define __lock_try_acquire_recursive(lock) __patmos_lock_try_acquire_recursive(&(lock))
#define __lock_release(lock)               __patmos_lock_release(&(lock))
#define __lock_release_recursive(lock)     __patmos_lock_release_recursive(&(lock))


extern int __patmos_lock_init(_LOCK_T *lock);
extern int __patmos_lock_init_recursive(_LOCK_RECURSIVE_T *lock);
extern int __patmos_lock_close(_LOCK_T *lock);
extern int __patmos_lock_close_recursive(_LOCK_RECURSIVE_T *lock);
extern int __patmos_lock_acquire(_LOCK_T *lock);
extern int __patmos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock);
extern int __patmos_lock_try_acquire(_LOCK_T *lock);
extern int __patmos_lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock);
extern int __patmos_lock_release(_LOCK_T *lock);
extern int __patmos_lock_release_recursive(_LOCK_RECURSIVE_T *lock);

#else /* __rtems__ */

/* TODO: these are just dummies, replace with actual RTEMS locking functions */

typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;
 
#include <_ansi.h>

#define __LOCK_INIT(class,lock) static int lock = 0;
#define __LOCK_INIT_RECURSIVE(class,lock) static int lock = 0;
#define __lock_init(lock) (_CAST_VOID 0)
#define __lock_init_recursive(lock) (_CAST_VOID 0)
#define __lock_close(lock) (_CAST_VOID 0)
#define __lock_close_recursive(lock) (_CAST_VOID 0)
#define __lock_acquire(lock) (_CAST_VOID 0)
#define __lock_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_try_acquire(lock) (_CAST_VOID 0)
#define __lock_try_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_release(lock) (_CAST_VOID 0)
#define __lock_release_recursive(lock) (_CAST_VOID 0)

#endif

#endif /* __SYS_LOCK_H__ */

