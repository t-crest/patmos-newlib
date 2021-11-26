// Copyright 2012 Technical University of Denmark, DTU Compute
// Author: Florian Brandner
//
// This file is part of the newlib C library for the Patmos processor.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//
//      1. Redistributions of source code must retain the above copyright notice,
//         this list of conditions and the following disclaimer.
//
//      2. Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
//   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
//   NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   The views and conclusions contained in the software and documentation are
//   those of the authors and should not be interpreted as representing official
//   policies, either expressed or implied, of the copyright holder.

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <reent.h>

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

void __corethread_worker(void) __attribute__((noinline));

//******************************************************************************

/// main - main function.
/// Note: LLVM currently silently ignores attributes for extern functions!
extern int main(int argc, char **argv) __attribute__((noinline));

/// memset - set memory content.
void *memset(void *s, int c, size_t n);

/// atexit - register call backs on program termination.
extern int atexit(void (*function)(void));

/// exit - terminate program.
extern void exit(int status) __attribute__((noinline));

//******************************************************************************
/// _loader_baseaddr - the base address of the loading function (one per core)
unsigned _loader_baseaddr[MAX_CORES];
/// _loader_off - the offset of the loading function (one per core)
unsigned _loader_off[MAX_CORES];

/// _reent_ptr - data structure for reentrant library calls
struct _reent *_reent_ptr [MAX_CORES];
/// __initreent - initialize reentrancy structure
void __initreent(void) __attribute__((noinline));


/// __init_exceptions - install default exception handlers
void  __init_exceptions(void);
/// __default_exc_handler - a default exception handler
void __default_exc_handler(void);

// Forward declaration of __start for _start.
void __start();

//******************************************************************************
/// _start - main entry function to all patmos executables.
/// Setup the stack frame and invoke __start.
void _start() __attribute__((naked,used));
void _start()
{

  asm volatile (
    // ---------------------------------------------------------------------------
    // Retrieve the id of the current core.
    "li $r1 = %[core_id];"
    "lwl $r1 = [$r1];"	// r1 = id
    // ---------------------------------------------------------------------------
    // setup stack frame and stack cache.
    "li $r2 = _stack_cache_base;"
    "li $r3 = _shadow_stack_base;"
    // compute effective stack addresses (needed for CMPs)
    "sub $r4 = $r2, $r3;" // stack size

    // make sure to have a positive stack size
    // workaround for -O0: avoid branch, perform abs(stack_size) via bit twiddling
    "sra $r5 = $r4, 31;" // mask
    "add $r4 = $r4, $r5;" // stack_size + mask
    "xor $r4 = $r4, $r5;" // stack_size = (stack_size + mask) ^ mask

    "mul $r1, $r4;"
    "nop;"
    "mfs $r4 = $sl;"
    "sl $r4 = $r4, 1;" // r4 = 2*stack_size*id

    "sub $r5 = $r3, $r4;" // = shadow_stack_base = &_shadow_stack_base - 2*stack_size*id
    "sub $r6 = $r2, $r4;" // = stack_cache_base = &_stack_cache_base - 2*stack_size*id

    "mov $r31 = $r5;" // initialize shadow stack pointer
    "mts $ss  = $r6;" // initialize the stack cache's spill pointer
    "mts $st  = $r6;" // initialize the stack cache's top pointer

    // ---------------------------------------------------------------------------
    // store return information of caller
    "li $r2 = _loader_baseaddr;"
    "li $r3 = _loader_off;"
    "shadd2 $r2 = $r2, $r1;" // r2 = &_loader_baseaddr[id]
    "shadd2 $r3 = $r3, $r1;" // r3 = &_loader_off[id]
    "mfs $r29 = $srb;"
    "swm [$r2] = $r29;"
    "mfs $r29 = $sro;"
    "swm [$r3] = $r29;"

    // ---------------------------------------------------------------------------
    // continue in __start

    // We use asm here to prevent LLVM from messing with the stack.
    // Calling (or actually any kind of more complex C code) is not supported by
    // the compiler, since it does not manage the stack in naked functions. If it 
    // spills registers at -O0, this can lead to stack corruption.
    // It would work in this case since it is a non-returning tail call without
    // arguments, but we should rather be defensive in system code.
    // If LLVM would inline the call, we could get code requiring spills into this
    // function (but the noinline attribute prevents this anyway).
    // If we need to analyse this code, we can easily extend the compiler and
    // platin to support inline asm (and we should, anyway!).
    "li $r1 = %[next_fn];"
    "call $r1;"	// resume in the no-return __start function 
    "nop  ;"
    "nop  ;"
    "nop  ;"	// no need for a 'ret'
    :
    : [core_id] "i" (__PATMOS_CPUINFO_COREID), [next_fn] "i" (__start)
    : "$r1", "$r2", "$r3", "$r4", "$r5", "$r6"
  );
}

/// __start - Main driver for program setup and execution.
/// Initialize data structures, invoke main, et cetera.
/// TODO needs a better name to make it more distinguishable from _start.
void __start() __attribute__((noinline));
void __start()
{
  // ---------------------------------------------------------------------------  
  // clear the BSS section
  // memset(&__bss_start, 0, &_end - &__bss_start);

  // ---------------------------------------------------------------------------  
  // initialize reentrancy structure
  __initreent();

  // ---------------------------------------------------------------------------  
  // install basic exception handlers
  __init_exceptions();

  // ---------------------------------------------------------------------------  
  // call initializers
  __init();

  // register callback to fini
  atexit(&__fini);

  // retrieve the id of the current core
  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  if (id == 0) {
      
    // ---------------------------------------------------------------------------  
    // invoke main -- without command line options

  // LLVM does not attach function attributes to declarations, we cannot mark
  // main as noinline in this file. 
  // Thus, we again use inline asm here for the call, but this time to prevent
  // inlining *of main* to avoid breaking evaluation and debugging scripts.
  // We also keep the call to exit while we are at it, to make debugging traces
  // easier.
    asm volatile ("call %0;"        // invoke main function
                  "li   $r3 = 0;"   // argc
                  "li   $r4 = 0;"   // argv
                  "nop  ;"
                  "call %1;"        // terminate program and invoke exit
                  "mov  $r3 = $r1;" // get exit code (in delay slot)
                  "nop  ;"
                  "nop  ;"
                  : : "r" (&main), "r" (&exit)
                  : "$r1", "$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8",
                    "$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15", "$r16",
                    "$r17", "$r18", "$r19", "$r20");
  } else {    
    asm volatile ("call %0;"        // invoke __corethread_worker function
                  "nop;"
                  "nop;"
                  "nop  ;"
                  "call %1;"        // terminate program and invoke exit
                  "mov  $r3 = $r1;" // get exit code (in delay slot)
                  "nop  ;"
                  "nop  ;"
                   : : "r" (&__corethread_worker), "r" (&exit)
                  : "$r1", "$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8",
                    "$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15", "$r16",
                    "$r17", "$r18", "$r19", "$r20");

  }

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

/// __initreent - initialize reentrancy structure
void __initreent(void) {
  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  _reent_ptr[id] = malloc(sizeof(struct _reent));
  _REENT_INIT_PTR(_reent_ptr[id]);
}

/// __getreent - get reentrancy structure for current thread
struct _reent *__getreent(void)
{
  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  return _reent_ptr[id];
}

/// __init_exceptions - install default exception handlers
void  __init_exceptions(void) {
  int i;
  // only install handlers if in privileged mode
  if (*((_iodev_ptr_t)(__PATMOS_EXCUNIT_STATUS)) & 0x2) {
    for (i = 0; i < 32; i++) {
      *((_IODEV exc_handler_t *)(__PATMOS_EXCUNIT_VEC + 4*i)) = &__default_exc_handler;
    }
  }
}

/// __default_exc_handler - a default exception handler
void __default_exc_handler(void) {
  unsigned source = *((_iodev_ptr_t)(__PATMOS_EXCUNIT_SRC));

  unsigned base, off;
  asm volatile("mfs %0 = $sxb;"
               "mfs %1 = $sxo;"
               : "=r" (base), "=r" (off));

  const char *msg = "";
  switch(source) {
  case 0: msg = " (illegal operation)"; break;
  case 1: msg = " (illegal memory access)"; break;
  }

  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  // Use printf only if the cpu is the master
  if(id == 0){
    printf("Aborting: exception %d%s at %#010x\n", source, msg, base+off);
    abort();
  }
  

}

void __corethread_worker(void) {
  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));
  unsigned long long time;
  boot_info->slave[id].status = STATUS_RETURN;
    
  // Wait for corethread_create request or application exit
  while(boot_info->master.status != STATUS_RETURN) {
    // As long as the master is still executing, wait for a corethread to
    // be created and then execute it.
    if (boot_info->slave[id].funcpoint != NULL) {
      funcpoint_t funcpoint = boot_info->slave[id].funcpoint;
      boot_info->slave[id].return_val = -1;
      boot_info->slave[id].status = STATUS_INITDONE;
      (*funcpoint)((void*)boot_info->slave[id].param);
      boot_info->slave[id].status = STATUS_RETURN;
      while(boot_info->slave[id].funcpoint != NULL) {

      }
    }
    boot_info->slave[id].status = STATUS_RETURN;
    time = get_cpu_usecs();
    //while(get_cpu_usecs() < time+10) {
    //  // Wait for 10 micro seconds before checking again
    //}
  }
  boot_info->slave[id].status = STATUS_RETURN;
  return;
}
