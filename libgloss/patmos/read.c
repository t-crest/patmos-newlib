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

// defined in lseek.c
extern int  __patmos_stdin_offset;

//******************************************************************************
/// _read - read a file descriptor.
int _read(int file, char *buf, int len)
{
  // stdin reads from the UART by default
  if (file == STDIN_FILENO)
  {
    int i;
    int seen_newline = 0;

    // read data
    for(i = 0; i < len; i++)
    {
      int s, c;

      // return if nothing is available and we already have a full line
      __PATMOS_UART_STATUS(s);
      if ((s & (__PATMOS_UART_DAV | __PATMOS_UART_PAE)) == 0 && seen_newline)
      {
        break;
      }

      // wait for data to be available from the UART
      while ((s & (__PATMOS_UART_DAV | __PATMOS_UART_PAE)) == 0)
      {
        __PATMOS_UART_STATUS(s);
      }
      
      // simulator signals EOF via parity error
      if ((s & __PATMOS_UART_PAE) != 0) {
        break;
      }

      // read the data from the UART.
      __PATMOS_UART_RD_DATA(c);

      // copy data into the given buffer.
      *buf++ = c & 0xff;

      // stdin is usually line buffered, so stop reading on newline
      if (c == '\n')
      {
        seen_newline = 1;
      }
    }

    // keep track of offset for lseek
    __patmos_stdin_offset += i;

    // clear error code and return
    errno = 0;
    return i;
  }

  // TODO: implement for simulator target
  errno = EBADF;
  return -1;
}
