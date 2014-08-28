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

#include <stddef.h>
#include "patmos.h"

//******************************************************************************
/// start and end of BSS section
extern int __bss_start, _end;

/// top of stack cache and shadow stack
extern int _shadow_stack_base, _stack_cache_base;

//******************************************************************************
typedef void (*funptr_t)(void);

/// init - initializer, used to call static constructors.
extern funptr_t __init_array_begin[0];
extern funptr_t __init_array_end[0];
void __init(void) __attribute__((noinline));

/// fini - finalizer, used to call static destructors.
extern funptr_t __fini_array_begin[0];
extern funptr_t __fini_array_end[0];
void __fini(void) __attribute__((noinline));

//******************************************************************************
/// stack cache functions
extern void _sc_reserve() __attribute__((naked,used));
extern void _sc_ensure() __attribute__((naked,used));
extern void _sc_free() __attribute__((naked,used));

//******************************************************************************

/// main - main function.
extern int main(int argc, char **argv) __attribute__((noinline));

/// memset - set memory content.
void *memset(void *s, int c, size_t n);

/// atexit - register call backs on program termination.
extern int atexit(void (*function)(void));

/// exit - terminate program.
extern void exit(int status) __attribute__((noinline));

//******************************************************************************
/// __env - values of environment vairables.
char *__env[1] = {0};

/// environ - values of environment vairables.
char **environ = __env;

/// MAX_CORES - the maximum number of cores
#define MAX_CORES 64
/// _loader_baseaddr - the base address of the loading function (one per core)
unsigned _loader_baseaddr[MAX_CORES];
/// _loader_off - the offset of the loading function (one per core)
unsigned _loader_off[MAX_CORES];

//******************************************************************************
int stack_size;
int _addr_base_spm, _addr_base_ext;   
//******************************************************************************

/// _start - main entry function to all patmos executables.
/// Setup the stack frame, initialize data structures, invoke main, et cetera.
void _start() __attribute__((naked,used));

void _start() 
{
  // retrieve the id of the current core
  const int id = *((_iodev_ptr_t)(&_cpuinfo_base+0x0));

  // ---------------------------------------------------------------------------
  // store return information of caller
  asm volatile ("mfs $r29 = $srb;"
                "swm [%0] = $r29;"
                "mfs $r29 = $sro;"
                "swm [%1] = $r29;"
                : : "r" (&_loader_baseaddr[id]), "r" (&_loader_off[id]));

  // ---------------------------------------------------------------------------  
  // setup stack frame and stack cache.

  // compute effective stack addresses (needed for CMPs)
  int stack_size =
    (unsigned)&_stack_cache_base - (unsigned)&_shadow_stack_base;
  if (stack_size < 0) { // make sure to have a positive size
    stack_size = -stack_size;
   }
    
  const unsigned shadow_stack_base =
    (unsigned)&_shadow_stack_base - 2*stack_size*id;
  const unsigned stack_cache_base =
    (unsigned)&_stack_cache_base - 2*stack_size*id;

  // ---------------------------------------------------------------------------  
  // setup stack frame and stack cache.
  asm volatile ("mov $r31 = %0;" // initialize shadow stack pointer"
                "mts $ss  = %1;" // initialize the stack cache's spill pointer"
                "mts $st  = %1;" // initialize the stack cache's top pointer"
                 : : "r" (shadow_stack_base), "r" (stack_cache_base));

  // XXX software stack cache setup
  int* brk;
  brk = sbrk(0xffff);
  _addr_base_spm = stack_cache_base; // keep swsc sp
  _addr_base_ext = &brk - 0xffff;  // keep swsc m_top

  asm volatile ("mov $r27  = %0;" // XXX fix base value
                "mov $r28  = %1;" // XXX fix base value
                 : : "r" (_addr_base_spm), "r" (_addr_base_ext));



  // ---------------------------------------------------------------------------  
  // clear the BSS section
  // memset(&__bss_start, 0, &_end - &__bss_start);

  // ---------------------------------------------------------------------------  
  // call initializers
  __init();
 
  // register callback to fini
  atexit(&__fini);
  
  // ---------------------------------------------------------------------------  
  // invoke main -- without command line options
  // we use asm to prevent LLVM from inlining into a naked function here

  asm volatile ("call %0;"        // invoke main function
                "li   $r3 = 0;"   // argc
                "li   $r4 = 0;"   // argv
                "nop  ;"
                "call %1;"        // terminate program and invoke exit
                "mov  $r3 = $r1;" // get exit code (in delay slot)
                "nop  ;"
                "nop  ;"
                 : : "i" (&main), "i" (&exit));

  // ---------------------------------------------------------------------------
  // in case this returns
  while(1) /* do nothing */;
}

/// init - initializer, used to call static constructors.
void __init(void) {
  for (funptr_t *i = __init_array_begin; i < __init_array_end; i++) {
    (*i)();
  }
}

/// fini - finalizer, used to call static destructors.
void __fini(void) {
  for (funptr_t *i = __fini_array_end-1; i >= __fini_array_begin; --i) {
    (*i)();
  }
}





