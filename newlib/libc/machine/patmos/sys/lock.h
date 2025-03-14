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

#include <pthread_mutex.h>

typedef pthread_mutex_t _LOCK_T;
typedef pthread_mutex_t _LOCK_RECURSIVE_T;

#define __LOCK_INIT(class,lock) \
  class _LOCK_T lock = PTHREAD_MUTEX_NORMAL_INITIALIZER;
#define __LOCK_INIT_RECURSIVE(class,lock) \
  class _LOCK_RECURSIVE_T lock = PTHREAD_MUTEX_RECURSIVE_INITIALIZER;

#define __lock_init(lock)                  lock = PTHREAD_MUTEX_NORMAL_INITIALIZER
#define __lock_init_recursive(lock)        lock = PTHREAD_MUTEX_RECURSIVE_INITIALIZER
#define __lock_close(lock)                 pthread_mutex_destroy(&(lock))
#define __lock_close_recursive(lock)       pthread_mutex_destroy(&(lock))
#define __lock_acquire(lock)               pthread_mutex_lock(&(lock))
#define __lock_acquire_recursive(lock)     pthread_mutex_lock(&(lock))
#define __lock_try_acquire(lock)           pthread_mutex_trylock(&(lock))
#define __lock_try_acquire_recursive(lock) pthread_mutex_trylock(&(lock))
#define __lock_release(lock)               pthread_mutex_unlock(&(lock))
#define __lock_release_recursive(lock)     pthread_mutex_unlock(&(lock))

#endif /* __SYS_LOCK_H__ */

