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
  _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;
  for (unsigned i = 0; i < sizeof(ll->entering)/sizeof(ll->entering[0]); i++) {
    ll->entering[i] = 0;
  }
  for (unsigned i = 0; i < sizeof(ll->number)/sizeof(ll->number[0]); i++) {
    ll->number[i] = 0;
  }
  return 0;
}

int __patmos_lock_init_recursive(_LOCK_RECURSIVE_T *lock) {
  __lock_init(lock->lock);
  _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;
  ll->owner = 0;
  ll->depth = 0;
  return 0;
}

int __patmos_lock_close(_LOCK_T *lock) {
  return 0;
}

int __patmos_lock_close_recursive(_LOCK_RECURSIVE_T *lock) {
  __lock_close(lock->lock);
  return 0;
}

static unsigned max(_UNCACHED _LOCK_T *ll) {
  unsigned m = 0;
  for (unsigned i = 0; i < sizeof(ll->number)/sizeof(ll->number[0]); i++) {
    unsigned n = ll->number[i];
    m = n > m ? n : m;
  }
  return m;
}

int __patmos_lock_acquire(_LOCK_T *lock) {
  unsigned char id = get_cpuid();
  _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

  ll->entering[id] = 1;
  unsigned n = 1 + max(ll);
  ll->number[id] = n;
  ll->entering[id] = 0;

  for (unsigned j = 0; j < sizeof(ll->number)/sizeof(ll->number[0]); j++) {
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

  return 0;
}

int __patmos_lock_release(_LOCK_T *lock) {
  unsigned char id = get_cpuid();
  _UNCACHED _LOCK_T *ll = (_UNCACHED _LOCK_T *)lock;

  ll->number[id] = 0; // exit section

  return 0;
}

int __patmos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock) {
  unsigned char id = get_cpuid();
  _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

  if (ll->owner != id || ll->depth == 0) {
    __lock_acquire(lock->lock);
    ll->owner = id;
  }

  ll->depth++;

  return 0;
}

int __patmos_lock_release_recursive(_LOCK_RECURSIVE_T *lock) {
  _UNCACHED _LOCK_RECURSIVE_T *ll = (_UNCACHED _LOCK_RECURSIVE_T *)lock;

  ll->depth--;

  if (ll->depth == 0) {
    __lock_release(lock->lock);
  }

  return 0;
}
