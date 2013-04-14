// Copyright 2012 Florian Brandner
//           2013 Stefan Hepp
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
    
#include <setjmp.h>

// Note: This cannot be a .S file since we compile newlib as bitcode archives.
// Note: naked should imply noinline, just want to make sure. Must be a proper call 
//       as the compiler needs to generate sens afterwards.

int setjmp(jmp_buf env) __attribute__((naked,noinline));
int setjmp(jmp_buf env) 
{
    // set res to 0 in asm so that the compiler does not optimize it away
    int res;
    asm volatile (
       "swc  [%1 + 0] = $r20	    \n\
	swc  [%1 + 1] = $r21	    \n\
	swc  [%1 + 2] = $r22	    \n\
	swc  [%1 + 3] = $r23	    \n\
	swc  [%1 + 4] = $r24	    \n\
	swc  [%1 + 5] = $r25	    \n\
	swc  [%1 + 6] = $r26	    \n\
	swc  [%1 + 7] = $r27	    \n\
	swc  [%1 + 8] = $r28	    \n\
	swc  [%1 + 9] = $r29	    \n\
	swc  [%1 + 10] = $r30	    \n\
	swc  [%1 + 11] = $r31	    \n\
	mfs  $r9  = $s0		    \n\
	mfs  $r10 = $s2		    \n\
	mfs  $r11 = $s3		    \n\
	mfs  $r12 = $s5		    \n\
	mfs  $r13 = $s6		    \n\
	sub  $r12 = $r12, $r13      \n\
	swc  [%1 + 12] = $r9	    \n\
	swc  [%1 + 13] = $r10	    \n\
	swc  [%1 + 14] = $r11	    \n\
	swc  [%1 + 15] = $r12	    \n\
	swc  [%1 + 16] = $r13	    \n\
	clr  %0"
	: "=r" (res) : "r" (env)
    );
    return res;
}

void longjmp(jmp_buf env, int value) __attribute__((naked));
void longjmp(jmp_buf env, int value)
{
    // Restore all callee-saved registers, predicates and TOS of stack cache
    // The stack cache is restored by first spilling everything to main memory, 
    // setting both the top an spill pointer to old stack top, and then ensuring the old 
    // cache size.
    // TODO setting both s5 and s6 and spilling first might not be necessary, depending 
    //      on mts $s5 syntax.
    asm volatile (
       "lwc $r20 = [%0 + 0]  \n\
	lwc $r21 = [%0 + 1]  \n\
	lwc $r22 = [%0 + 2]  \n\
	lwc $r23 = [%0 + 3]  \n\
	lwc $r24 = [%0 + 4]  \n\
	lwc $r25 = [%0 + 5]  \n\
	lwc $r26 = [%0 + 6]  \n\
	lwc $r27 = [%0 + 7]  \n\
	lwc $r28 = [%0 + 8]  \n\
	lwc $r29 = [%0 + 9]  \n\
	lwc $r30 = [%0 + 10] \n\
	lwc $r31 = [%0 + 11] \n\
	mfs  $r12 = $s5	     \n\
	mfs  $r13 = $s6	     \n\
	sub  $r12 = $r12, $r13  \n\
	sspill $r12	     \n\
	lwc $r9  = [%0 + 12] \n\
	lwc $r10 = [%0 + 13] \n\
	lwc $r11 = [%0 + 14] \n\
	lwc $r12 = [%0 + 15] \n\
	lwc $r13 = [%0 + 16] \n\
	mts $s2 = $r10       \n\
	mts $s3 = $r11       \n\
	mts $s5 = $r13       \n\
	mts $s6 = $r13       \n\
	sens $r12	     \n\
	ret $r30, $r31	     \n\
	mts $s0 = $r9        \n\
	mov $r1 = %1"
	: : "r" (env), "r" (value)
    );

    // TODO mark as unreachable, so that the compiler does not insert a ret here
}

