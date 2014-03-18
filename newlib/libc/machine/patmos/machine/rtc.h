// Copyright 2013 Stefan Hepp
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
//
// This file contains Patmos RTC related functions. 
//
// TODO define helper functions to trigger *after* some time from now
//      (accounting for the time to setup the timer?), and to trigger
//      periodically (i.e., remember the last setup time of the timers).
//

#ifndef _MACHPATMOS_RTC_H
#define _MACHPATMOS_RTC_H

#include <machine/patmos.h>

/**
 * Base address of the RTC IO address range.
 * Defined by patmos-clang driver as a symbol at link-time
 */
extern char _timer_base;


/**
 * Get the current cycle counter value.
 */
static inline unsigned long long get_cpu_cycles(void) {
  unsigned clo, chi;

  // TODO this code is identical to libgloss/patmos/time.c, share code.

  _iodev_ptr_t hi_clock = (_iodev_ptr_t)(&_timer_base + 0x0);
  _iodev_ptr_t lo_clock = (_iodev_ptr_t)(&_timer_base + 0x4);

  // Order is important here
  clo = *lo_clock;
  chi = *hi_clock;

  return (((unsigned long long) chi) << 32) | clo;
}

/**
 * Get the current RTC microsecond value.
 */
static inline unsigned long long get_cpu_usecs(void) {
  unsigned ulo, uhi;

  // TODO this code is identical to libgloss/patmos/time.c, share code.

  _iodev_ptr_t hi_usec = (_iodev_ptr_t)(&_timer_base + 0x8);
  _iodev_ptr_t lo_usec = (_iodev_ptr_t)(&_timer_base + 0xc);

  // Order is important here
  ulo = *lo_usec;
  uhi = *hi_usec;

  return (((unsigned long long) uhi) << 32) | ulo;
}

/**
 * Set the timeout for the clock timer. The RTC will trigger an interrupt once
 * the cycle counter reaches the given value.
 */
static inline void arm_clock_timer(unsigned long long timestamp) {
  unsigned clo, chi;

  _iodev_ptr_t hi_clock = (_iodev_ptr_t)(&_timer_base + 0x0);
  _iodev_ptr_t lo_clock = (_iodev_ptr_t)(&_timer_base + 0x4);

  // Order is important here
  *lo_clock = (unsigned)timestamp;
  *hi_clock = (unsigned)(timestamp>>32);
}

/**
 * Set the timeout for the clock timer. The RTC will trigger an interrupt once
 * the cycle counter reaches the given value.
 */
static inline void arm_usec_timer(unsigned long long timestamp) {
  unsigned clo, chi;

  _iodev_ptr_t hi_usec = (_iodev_ptr_t)(&_timer_base + 0x8);
  _iodev_ptr_t lo_usec = (_iodev_ptr_t)(&_timer_base + 0xc);

  // Order is important here
  *lo_usec = (unsigned)timestamp;
  *hi_usec = (unsigned)(timestamp>>32);
}


#endif /* _MACHPATMOS_RTC_H */
