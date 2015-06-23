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
 */
#define __PATMOS_TIMER_BASE       0xF0020000
#define __PATMOS_TIMER_HICLK      (__PATMOS_TIMER_BASE + 0x00)
#define __PATMOS_TIMER_LOCLK      (__PATMOS_TIMER_BASE + 0x04)
#define __PATMOS_TIMER_HIUSEC     (__PATMOS_TIMER_BASE + 0x08)
#define __PATMOS_TIMER_LOUSEC     (__PATMOS_TIMER_BASE + 0x0c)


/**
 * Get the current cycle counter value.
 */
static inline unsigned long long get_cpu_cycles(void) {
  unsigned clo, chi;

  // TODO this code is identical to libgloss/patmos/time.c, share code.

  // Prevent the compiler from moving the read over other instructions 
  // or into a call delay slot behind the call miss stall
  asm volatile ("" : : : "memory");

  _iodev_ptr_t hi_clock = (_iodev_ptr_t)(__PATMOS_TIMER_HICLK);
  _iodev_ptr_t lo_clock = (_iodev_ptr_t)(__PATMOS_TIMER_LOCLK);

  // Order is important here
  clo = *lo_clock;
  chi = *hi_clock;

  asm volatile ("" : : : "memory");

  return (((unsigned long long) chi) << 32) | clo;
}

/**
 * Get the current RTC microsecond value.
 */
static inline unsigned long long get_cpu_usecs(void) {
  unsigned ulo, uhi;

  // TODO this code is identical to libgloss/patmos/time.c, share code.

  // Prevent the compiler from moving the read over other instructions 
  // or into a call delay slot behind the call miss stall
  asm volatile ("" : : : "memory");

  _iodev_ptr_t hi_usec = (_iodev_ptr_t)(__PATMOS_TIMER_HIUSEC);
  _iodev_ptr_t lo_usec = (_iodev_ptr_t)(__PATMOS_TIMER_LOUSEC);

  // Order is important here
  ulo = *lo_usec;
  uhi = *hi_usec;

  asm volatile ("" : : : "memory");

  return (((unsigned long long) uhi) << 32) | ulo;
}

/**
 * Set the timeout for the clock timer. The RTC will trigger an interrupt once
 * the cycle counter reaches the given value.
 */
static inline void arm_clock_timer(unsigned long long timestamp) {
  unsigned clo, chi;

  // Prevent the compiler from moving the write over other instructions 
  // or into a call delay slot behind the call miss stall
  asm volatile ("" : : : "memory");

  _iodev_ptr_t hi_clock = (_iodev_ptr_t)(__PATMOS_TIMER_HICLK);
  _iodev_ptr_t lo_clock = (_iodev_ptr_t)(__PATMOS_TIMER_LOCLK);

  // Order is important here
  *lo_clock = (unsigned)timestamp;
  *hi_clock = (unsigned)(timestamp>>32);

  asm volatile ("" : : : "memory");
}

/**
 * Set the timeout for the clock timer. The RTC will trigger an interrupt once
 * the cycle counter reaches the given value.
 */
static inline void arm_usec_timer(unsigned long long timestamp) {
  unsigned clo, chi;

  // Prevent the compiler from moving the write over other instructions 
  // or into a call delay slot behind the call miss stall
  asm volatile ("" : : : "memory");

  _iodev_ptr_t hi_usec = (_iodev_ptr_t)(__PATMOS_TIMER_HIUSEC);
  _iodev_ptr_t lo_usec = (_iodev_ptr_t)(__PATMOS_TIMER_LOUSEC);

  // Order is important here
  *lo_usec = (unsigned)timestamp;
  *hi_usec = (unsigned)(timestamp>>32);

  asm volatile ("" : : : "memory");
}


#endif /* _MACHPATMOS_RTC_H */
