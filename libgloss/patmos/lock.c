// Copyright 2014  Technical University of Denmark, DTU Compute.
//
// This file is part of the newlib C library for the Patmos processor.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//
//      1. Redistributions of source code must retain the above copyright notice,
//         this list of conditions and the following disclaimer.
//
//      2. Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
//   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
//   NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   The views and conclusions contained in the software and documentation are
//   those of the authors and should not be interpreted as representing official
//   policies, either expressed or implied, of the copyright holder.

#include <string.h>
#include <sys/lock.h>

#include <machine/patmos.h>

/* This locking implementation uses a single global hardware lock when
   when modifying the lock structures.
 */
 
// Assumes that the Hardlock is connected with at least 1 lock, used as a global lock

#ifndef _HARDLOCK_LOCK
#define _HARDLOCK_LOCK() do {asm volatile ("" : : : "memory"); *((_iodev_ptr_t) PATMOS_IO_HARDLOCK) = 1; asm volatile ("" : : : "memory");} while(0)
#endif
#ifndef _HARDLOCK_UNLOCK
#define _HARDLOCK_UNLOCK() do {asm volatile ("" : : : "memory"); *((_iodev_ptr_t) PATMOS_IO_HARDLOCK) = 0; asm volatile ("" : : : "memory");} while(0)
#endif

int __patmos_lock_init(_LOCK_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;
    ll->owner = __EMPTY_LOCK;
    ll->ticket_req = 0;
    ll->ticket_cur = 0;
  }
  return 0;
}

int __patmos_lock_init_recursive(_LOCK_RECURSIVE_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    __lock_init(lock->lock);
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;
    ll->depth = 0;
  }
  return 0;
}

int __patmos_lock_close(_LOCK_T *lock) {
  return 0;
}

int __patmos_lock_close_recursive(_LOCK_RECURSIVE_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1)
    __lock_close(lock->lock);
  return 0;
}

int __patmos_lock_acquire(_LOCK_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    const int id = get_cpuid();
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

    if(ll->owner == id)
      return 0;

    _HARDLOCK_LOCK();
    int ticket = ll->ticket_req++;
    _HARDLOCK_UNLOCK();

    while(1)
      if(ticket == ll->ticket_cur)
        break;

    ll->owner = id;

    // invalidate data cache to establish cache coherence
    inval_dcache();
  }
  return 0;
}

int __patmos_lock_release(_LOCK_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    const int id = get_cpuid();
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

    ll->owner = __EMPTY_LOCK;
    ll->ticket_cur++;
  }
  return 0;
}

int __patmos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    const int id = get_cpuid();
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

    if (ll->lock.owner != id)
      return __lock_acquire(lock->lock);

    ll->depth++;
  }
  return 0;
}

int __patmos_lock_release_recursive(_LOCK_RECURSIVE_T *lock) {
  const int cnt = get_cpucnt();
  if (cnt > 1) {
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

    if (ll->depth == 0)
      return __lock_release(lock->lock);

    ll->depth--;
  }
  return 0;
}
