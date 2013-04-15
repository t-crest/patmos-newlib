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
// TODO maybe move stuff to separate headers? Too lazy for now.
//

#ifndef _MACHPATMOS_H
#define _MACHPATMOS_H

/**
 * Base address of the IO address range.
 * Defined by patmos-clang driver as a symbol at link-time
 */
extern unsigned int _iomap_base;

/**
 * Get the CPU ID.
 */
static inline unsigned int get_cpuid()
{
   unsigned int id;
   asm ("lwl %0 = [%1]" : "=r" (id) : "r" (&_iomap_base));
   return id;
}

/**
 * Invalidate the data cache state.
 */
static inline void invalidate_data_cache()
{
    // TODO implement
}


#endif /* _MACHPATMOS_H */
