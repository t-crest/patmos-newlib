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

//******************************************************************************
/// start and end of BSS section
extern int __bss_start, _end;

/// top of stack cache and shadow stack
extern int _shadow_stack_base, _stack_cache_base;

//******************************************************************************
/// init - initializer, usually used to call C++ constructors.
extern void __init();

/// fini - finalizer, usually used to call C++ destructors.
extern void __fini();

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


//******************************************************************************
/// _start - main entry function to all patmos executables.
/// Setup the stack frame, initialize data structures, invoke main, et cetera.
void _start() __attribute__((naked,used));

void _start()
{
  // ---------------------------------------------------------------------------  
  // setup stack frame and stack cache.
  asm volatile ("mov $r29 = %0;" // initialize shadow stack pointer"
                "mts $ss  = %1;" // initialize the stack cache's spill pointer"
                "mts $st  = %1;" // initialize the stack cache's top pointer"
                 : : "r" (&_shadow_stack_base), "r" (&_stack_cache_base));

  // ---------------------------------------------------------------------------  
  // clear the BSS section
  // memset(&__bss_start, 0, &_end - &__bss_start);

  // ---------------------------------------------------------------------------  
  // call C++ initializer
  // TODO: enable
  // __init();
 
  // register callback to fini
  // TODO: enable
  // atexit(&__fini);
  
  // ---------------------------------------------------------------------------  
  // invoke main -- without command line options
  // we use asm to prevent LLVM from inlining into a naked function here
  
  asm volatile ("call %1;"        // invoke main function
		"li   $r30 = %0;" // set function base (in delay slot)
		"li   $r3 = 0;"   // argc
                "li   $r4 = 0;"   // argv
                "call %2;"        // terminate program and invoke exit
                "mov  $r3 = $r1;" // get exit code (in delay slot)
                "nop  ;"
                "nop  ;"
                 : : "i" (&_start), "i" (&main), "i" (&exit));

  // ---------------------------------------------------------------------------
  // in case this returns
  while(1) /* do nothing */;
}
