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
void _exit(int status)
{
  asm volatile ("mov $r1 = %0;;"  // store exit code
                "mov $r30 = $r0;;" // clear sb and return == halt
                "ret $r0, $r0;;"
		"nop;;"
		"nop;;"
                 : : "r" (status));
  while (1) /* do nothing */;
}
