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
/// _write - write to a file descriptor.
int _write(int file, char *buf, int nbytes)
{
  // stdout writes to the UART by default
  //if (file == STDOUT_FILENO || file == STDERR_FILENO)
  if (file == STDOUT_FILENO)
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
  errno  = EBADF;
  return -1;
}
