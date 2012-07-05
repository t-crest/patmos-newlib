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

//******************************************************************************
/// _read - read a file descriptor.
int _read(int file, char *buf, int len)
{
  // stdin reads from the UART by default
  if (file == STDIN_FILENO)
  {
    int i;

    // read data
    for(i = 0; i < len; i++)
    {
      char s, c;

      // wait for data to be available from the UART
      do
      {
        __PATMOS_UART_STATUS(s);
      } while((s & __PATMOS_UART_DAV) == 0);

      // read the data from the UART.
      __PATMOS_UART_RD_DATA(c);

      // copy data into the given buffer.
      *buf++ = c;
    }

    // clear error code and return
    errno = 0;
    return len;
  }

  // TODO: implement for simulator target
  errno = EBADF;
  return -1;
}
