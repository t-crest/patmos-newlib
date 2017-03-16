// Copyright 2012 Technical University of Denmark, DTU Compute
// Author: Florian Brandner
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
//
// Some Patmos specific defines (e.g., concerning the UART coming with Patmos).

#ifndef __PATMOS__H
#define __PATMOS__H

#define _IODEV __attribute__((address_space(1)))

typedef _IODEV unsigned int volatile * const _iodev_ptr_t;

/// The base address of the cpuinfo device
#define __PATMOS_CPUINFO_BASE   0xF0000000
#define __PATMOS_CPUINFO_COREID  (__PATMOS_CPUINFO_BASE + 0x00)

/// The base address of the exception unit
#define __PATMOS_EXCUNIT_BASE   0xF0010000
#define __PATMOS_EXCUNIT_STATUS (__PATMOS_EXCUNIT_BASE + 0x00)
#define __PATMOS_EXCUNIT_SRC    (__PATMOS_EXCUNIT_BASE + 0x0c)
#define __PATMOS_EXCUNIT_VEC    (__PATMOS_EXCUNIT_BASE + 0x80)

/// The exception handler type.
typedef void (*exc_handler_t)(void);

/// The base address of the timer device
#define __PATMOS_TIMER_BASE     0xF0020000
#define __PATMOS_TIMER_HIUSEC     (__PATMOS_TIMER_BASE + 0x08)
#define __PATMOS_TIMER_LOUSEC     (__PATMOS_TIMER_BASE + 0x0c)

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
 * Attribute for pointers into main memory using cache bypass. Use as
 *
 * _UNCACHED int *p = (_UNCACHED int *) &mydata;
 */
#define _UNCACHED __attribute__((address_space(3)))


#define MAX_CORES 64

#define STATUS_NULL     0
#define STATUS_BOOT     1
#define STATUS_INIT     2
#define STATUS_INITDONE 3
#define STATUS_RETURN   4

#ifndef __ENTRYPOINT_T
typedef volatile int (*entrypoint_t)(void);
#define __ENTRYPOINT_T
#endif

#ifndef __FUNCPOINT_T
typedef volatile void (*funcpoint_t)(void*);
#define __FUNCPOINT_T
#endif

struct master_info_t {  
  volatile entrypoint_t entrypoint;
  volatile int status;
};

struct slave_info_t {
  volatile funcpoint_t funcpoint;
  volatile void* param;
  volatile int return_val;
  volatile int status;
};

struct boot_info_t {
  struct master_info_t master;
  struct slave_info_t slave[MAX_CORES];
};

/* Place boot info at the beginning of the memory. Nothing else may be
   placed there. */
#define boot_info ((_UNCACHED struct boot_info_t *)0x00000010)

/// The address of the UART device
#define __PATMOS_UART_BASE      0xF0080000

/// Bit mask for the transmit-ready bit (TRE)
#define __PATMOS_UART_TRE 1

/// Bit mask for the data-available bit (DAV)
#define __PATMOS_UART_DAV 2

/// Bit mask for the parity-error bit (PAE)
#define __PATMOS_UART_PAE 4

/// Address to access the status register of the UART coming with Patmos
#define __PATMOS_UART_STATUS_ADDR (__PATMOS_UART_BASE+0x0)

/// Address to access the data register of the UART coming with Patmos
#define __PATMOS_UART_DATA_ADDR (__PATMOS_UART_BASE+0x04)

/// Macro to read the UART's status register
#define __PATMOS_UART_STATUS(res) res = *((_iodev_ptr_t)__PATMOS_UART_STATUS_ADDR);

/// Macro to read the UART's data register
#define __PATMOS_UART_RD_DATA(res) res = *((_iodev_ptr_t)__PATMOS_UART_DATA_ADDR);

/// Macro to write the UART's control register
#define __PATMOS_UART_WR_CTRL(data) *((_iodev_ptr_t)__PATMOS_UART_STATUS_ADDR) = data;
 
/// Macro to write the UART's data register
#define __PATMOS_UART_WR_DATA(data) *((_iodev_ptr_t)__PATMOS_UART_DATA_ADDR) = data;


#endif // __PATMOS__H
