// Copyright 2014 Sahar Abbaspour
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

#include <stdio.h>
#include <machine/patmos.h>
#include <machine/spm.h>

/// get stack cache size from crt0
extern int stack_size;
//extern int _sc_size() __attribute__((naked,used));;

void _sc_reserve() __attribute__((naked,used));
void _sc_ensure() __attribute__((naked,used));
void _sc_free() __attribute__((naked,used));



/// function argument (words) is passed in scratch r1

void _sc_reserve()
{
  // some counter
  int n, n_spill, i;
   _SPM unsigned int *sc_top;
   _UNCACHED unsigned int *m_top;

  asm("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
     // : "=r" (n) 
      : "=r" (n), "=r"(sc_top), "=r"(m_top));

  // n_spill = m_top - sc_top - stack_size;

   for (i = 0; i < 5; i++){
        
	//m_top -= 0x01;
	*m_top = *sc_top;
         
  }




}



//
// ensure/free cannot use r1, this would clobber the result, use an arg
// register? r8?
//
void _sc_ensure() {
}

void _sc_free()  {
#if 0
  int i;
  // copy argument to i
  asm volatile("mov %0 = $r8"
      : "=r" (i));


  // iterate something
  while(1) {
    if (!i)
      break;
    i--;
    asm("nop"); // this is something
  }
#endif
}
