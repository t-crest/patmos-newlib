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
