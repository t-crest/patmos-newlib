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
// This file contains various Patmos related functions. 
//

#ifndef _MACHPATMOS_H
#define _MACHPATMOS_H

/**
 * Base address of the CPU info device.
 * Defined by patmos-clang driver as a symbol at link-time
 */
extern char _cpuinfo_base;

/**
 * Attribute for pointers into the IO-mapped memory. Use as
 *
 * _IODEV int *p = (_IODEV int *) 0x1234;
 *
 * @see _SPM in machine/spm.h
 */
#define _IODEV __attribute__((address_space(1)))

typedef _IODEV unsigned int volatile * const _iodev_ptr_t;

/**
 * Attribute for pointers into main memory using cache bypass. Use as
 *
 * _UNCACHED int *p = (_UNCACHED int *) &mydata;
 */
#define _UNCACHED __attribute__((address_space(3)))


/**
 * Get the CPU ID.
 */
static inline unsigned int get_cpuid()
{
  unsigned int id = *((_iodev_ptr_t)(&_cpuinfo_base+0x0));
  return id;
}

/**
 * Get the CPU frequency.
 */
static inline unsigned int get_cpu_freq()
{
  unsigned int freq = *((_iodev_ptr_t)(&_cpuinfo_base+0x4));
  return freq;
}


/**
 * Base address of the exception unit.
 */
extern char _excunit_base;

/**
 * The cache control register (in the exception unit).
 */
#define CACHE_CONTROL (*((_iodev_ptr_t)(&_excunit_base+0x14)))

/**
 * Invalidate the data cache.
 */
static inline void inval_dcache()
{
  CACHE_CONTROL = 0x01;
}

/**
 * Invalidate the method cache.
 */
static inline void inval_mcache()
{
  CACHE_CONTROL = 0x02;
}

#endif /* _MACHPATMOS_H */
