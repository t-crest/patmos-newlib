// Copyright 2012 Technical University of Denmark, DTU Compute
// Author: Florian Brandner
// 
// This file is part of the newlib C library for the Patmos processor.
// 
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
//******************************************************************************
/// _exit - halt the processor.

#include "patmos.h"

extern unsigned _loader_baseaddr [];
extern unsigned _loader_off [];

void _exit(int status)
{
  // retrieve the id of the current core
  const int id = *((_iodev_ptr_t)(__PATMOS_CPUINFO_COREID));

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
