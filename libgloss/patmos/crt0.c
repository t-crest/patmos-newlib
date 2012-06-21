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
/// start and end of BSS section
extern int __bss_start, __bss_end;

//******************************************************************************
/// init - initializer, usually used to call C++ constructors.
extern void __init();

/// fini - finalizer, usually used to call C++ destructors.
extern void __fini();

//******************************************************************************

/// main - main function.
extern int main(int argc, char **argv);

/// atexit - register call backs on program termination.
extern int atexit(void (*function)(void));

/// exit - terminate program.
extern void _exit(int status);

//******************************************************************************
/// __env - values of environment vairables.
char *__env[1] = {0};

/// environ - values of environment vairables.
char **environ = __env;


//******************************************************************************
/// _start - main entry function to all patmos executables.
/// Setup the stack frame, initialize data structures, invoke main, et cetera.
void _start() __attribute__((naked));

void _start()
{
  // ---------------------------------------------------------------------------  
  // setup stack frame and stack cache.
  asm("mov $r31 = _shadow_stack_base ; # initialize shadow stack pointer\n\t"
      "mov $r1  = _stack_cache_base    # initialize the stack cache's top pointer\n\t"
      "mts $st  = $r1");
  
  // ---------------------------------------------------------------------------  
  // clear the BSS section
  for(int *i = &__bss_start, *ie = &__bss_end; i != ie; i++)
  {
    *i = 0;
  }
  
  // ---------------------------------------------------------------------------  
  // call C++ initializer
  // TODO: enable
  // __init();
 
  // register callback to fini
  // TODO: enable
  // atexit(&__fini);
  
  // ---------------------------------------------------------------------------  
  // invoke main -- without command line options
  // TODO: implement this for the simulator target
  int r = main(0, 0);
  
  // ---------------------------------------------------------------------------  
  // done, terminate program
  _exit(r);
  
  // in case this returns
  while(1) /* do nothing */;
}
