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
    
//******************************************************************************
/// _exit - halt the processor.

#include "patmos.h"

extern unsigned _loader_baseaddr [];
extern unsigned _loader_off [];

void _exit(int status)
{
  // retrieve the id of the current core
  const int id = *((_iodev_ptr_t)(&_cpuinfo_base+0x0));

  // return to loader; halts if baseaddr and off are 0
  asm volatile ("mts $srb = %0;"
                "mts $sro = %1;"
                "mov $r1 = %2;"  // store exit code
                "ret;"
                "nop;"
                "nop;"
                "nop;"
                : : "r" (_loader_baseaddr[id]), "r" (_loader_off[id]), "r" (status));
}
