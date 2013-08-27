// Copyright 2013 Technical University of Denmark, DTU Compute. 
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
// This file contains Patmos-specific definitions for exceptions and interrupts
//
// Author: Wolfgang Puffitsch (wpuffitsch@gmail.com)

#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include "patmos.h"

/// linker symbol giving the address of the exception unit
extern char _excunit_base;

#define EXC_STATUS (*((_iodev_ptr_t)(&_excunit_base+0x00)))
#define EXC_MASK   (*((_iodev_ptr_t)(&_excunit_base+0x04)))
#define EXC_PEND   (*((_iodev_ptr_t)(&_excunit_base+0x08)))
#define EXC_SOURCE (*((_iodev_ptr_t)(&_excunit_base+0x0c)))

/// function type of exception handlers
typedef void (*exc_handler_t)(void);

#define EXC_VEC_LENGTH 32
#define EXC_VEC(I) (((_IODEV exc_handler_t volatile * const)(&_excunit_base+0x80))[I])

/// enable interrupts
static inline void sei(void) {
  EXC_STATUS |= 0x01;
}
/// disable interrupts
static inline void cli(void) {
  EXC_STATUS &= ~0x01;
}

/// register exception handler
static inline void exception(unsigned int i, exc_handler_t fun) {
  EXC_VEC(i) = fun;
}

#endif /* _EXCEPTIONS_H */
