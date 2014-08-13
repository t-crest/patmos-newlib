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

#include <stddef.h>
#include <stdint.h>
#include "patmos.h"

void _sc_reserve() __attribute__((naked,used));
void _sc_ensure() __attribute__((naked,used));
void _sc_free() __attribute__((naked,used));

/// function argument (words) is passed in scratch r1
void _sc_reserve()
{
  // some counter
  int i, nspill;

  // copy argument to i
  asm("mov %0 = $r1"
      : "=r" (i) 
      : "r" (i));

  // XXX is this enough to reserve R9/R10 throughout the whole function?
  asm volatile ("" ::: "$r3", "$r4", "$r5", "$r6", "$r7", "$r8", "$r9", "$r10");
   

 /* for (uint32_t x=0; x < ; x++)
        {
           // Assumes dwSomeValue is not zero.
           asm ("bsfl %1,%0"
             : "=r" (dwRes)
             : "r" (dwSomeValue)
             : "cc");
     
           printf("%u: %u %u\n", x, dwSomeValue, dwRes);
        }*/

 /* // iterate something
  while(1) {
    if (!i)
      break;
    i--;
    asm("nop"); // this is something
  }*/

}

//
// ensure/free cannot use r1, this would clobber the result, use an arg
// register? r8?
//
void _sc_ensure() {
}

void _sc_free()  {
  int i;
  // copy argument to i
  asm volatile("mov %0 = $r8"
      : "=r" (i));

  // XXX is this enough to reserve R9/R10 throughout the whole function?
  // XXX we must not clobber the return regs at this point
  asm volatile ("" ::: "$r1", "$r2", "$r9", "$r10");

  // iterate something
  while(1) {
    if (!i)
      break;
    i--;
    asm("nop"); // this is something
  }
}
