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

/* This locking implementation uses Lamport's baker algorithm.
 *
 * Rationale: It is faster than Peterson's generalized algorithm for N
 * threads. Lamport's "fast" algorithm would be much faster for
 * uncontended locks, but is not starvation-free.
 *
 * Caveat: The "number" field in the locking structure may be
 * unbounded if a lock is continuously acquired/released without ever
 * being free.
 */

int __patmos_lock_init(_LOCK_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;
    for (unsigned i = 0; i < cnt; i++) {
      ll->entering[i] = 0;
    }
    for (unsigned i = 0; i < cnt; i++) {
      ll->number[i] = 0;
    }
  }
  return 0;
}

int __patmos_lock_init_recursive(_LOCK_RECURSIVE_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    __lock_init(lock->lock);
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;
    ll->owner = -1;
    ll->depth = 0;
  }
  return 0;
}

int __patmos_lock_close(_LOCK_T *lock) {
  return 0;
}

int __patmos_lock_close_recursive(_LOCK_RECURSIVE_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    __lock_close(lock->lock);
  }
  return 0;
}

static unsigned max(_UNCACHED _LOCK_T *ll) {
  const unsigned cnt = get_cpucnt();
  unsigned m = 0;
  for (unsigned i = 0; i < cnt; i++) {
    unsigned n = ll->number[i];
    m = n > m ? n : m;
  }
  return m;
}

int __patmos_lock_acquire(_LOCK_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {

    const unsigned char id = get_cpuid();
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

    ll->entering[id] = 1;
    unsigned n = 1 + max(ll);
    ll->number[id] = n;
    ll->entering[id] = 0;

    for (unsigned j = 0; j < cnt; j++) {
      while (ll->entering[j]) {
        /* busy wait */
      }
      unsigned m = ll->number[j];
      while ((m != 0) &&
             ((m < n) || ((m == n) && (j < id)))) {
        /* busy wait, only update m */
        m = ll->number[j];
      }
    }

    // invalidate data cache to establish cache coherence
    inval_dcache();
  }

  return 0;
}

int __patmos_lock_release(_LOCK_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    const unsigned char id = get_cpuid();
    _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

    ll->number[id] = 0; // exit section
  }
  return 0;
}

int __patmos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    const unsigned char id = get_cpuid();
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

    if (ll->owner != id || ll->depth == 0) {
      __lock_acquire(lock->lock);
      ll->owner = id;
    }

    ll->depth++;
  }
  return 0;
}

int __patmos_lock_release_recursive(_LOCK_RECURSIVE_T *lock) {
  const unsigned cnt = get_cpucnt();
  if (cnt > 1) {
    _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

    ll->depth--;

    if (ll->depth == 0) {
      ll->owner = -1; // reset owner to invalid ID
      __lock_release(lock->lock);
    }
  }
  return 0;
}
