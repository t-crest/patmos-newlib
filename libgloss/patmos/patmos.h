// Copyright 2012 Florian Brandner
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
#define __PATMOS_EXCUNIT_SRC    (__PATMOS_EXCUNIT_BASE + 0x0c)
#define __PATMOS_EXCUNIT_VEC    (__PATMOS_EXCUNIT_BASE + 0x80)

/// The exception handler type.
typedef void (*exc_handler_t)(void);

/// The base address of the timer device
#define __PATMOS_TIMER_BASE     0xF0020000

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
