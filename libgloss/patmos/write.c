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

#include "patmos.h"
#include <errno.h>
#include <unistd.h>

#undef errno
extern int  errno;

//******************************************************************************
/// patmos-plug: write: stub called if 'patmosplug_write(int, char *, int)' is
/// not defined.
int _patmosplug_write(int file, char *buf, int nbytes) {
  errno = EBADF;
  return -1;
}

int patmosplug_write(int file, char *buf, int nbytes)
    __attribute__((weak, alias("_patmosplug_write")));

//******************************************************************************
/// _write - write to a file descriptor.
int _write(int file, char *buf, int nbytes)
{
  // stdout and stderr write both to the UART by default
  // TODO we could add a flag that can be set at runtime to disable stderr on UART,
  //      or we could insert some control bytes into the stream to switch between 
  //      stdout and stderr and use a small program on the host to demux the streams.
  //if (file == STDOUT_FILENO)
  if (file == STDOUT_FILENO || file == STDERR_FILENO)
  {
    int i;

    // read data
    for(i = 0; i < nbytes; i++)
    {
      int s, c;

      // wait for the UART to be ready for transmission
      do
      {
        __PATMOS_UART_STATUS(s);
      } while((s & __PATMOS_UART_TRE) == 0);

      // copy data into the given buffer.
      c = (*buf++) & 0xff;

      // write data to the UART.
      __PATMOS_UART_WR_DATA(c);
    }

    // clear error code and return
    errno = 0;
    return nbytes;
  }

  // TODO: implement for simulator target
  return patmosplug_write(file, buf, nbytes);
}
