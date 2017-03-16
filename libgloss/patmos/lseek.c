// Copyright 2012 Technical University of Denmark, DTU Compute
// Author: Florian Brandner
// Copyright 2012 TU Wien, Author: Stefan Hepp
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
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#undef errno
extern int  errno;

// Keep the offset in stdin so that we can return the correct value for lseek
// and support SEEK_SET (fseek with SEEK_CUR will always convert to SEEK_SET)
// TODO I guess there should be a nicer way to do this ?!
int __patmos_stdin_offset = 0;

//******************************************************************************
/// _lseek - seek position for a file descriptor.
int _lseek(int fd, off_t offset, int whenceq)
{
  // For stdin and (positive) seek relative to current pos, we just consume bytes
  if (fd == STDIN_FILENO) {
    int curoff;

    switch (whenceq) {
    case SEEK_SET: curoff = offset - __patmos_stdin_offset; break;
    case SEEK_CUR: curoff = offset; break;
    default:
	// We do not support SEEK_END for now
	errno = EINVAL;
	return -1;
    }
    if (curoff < 0) {
	// We do not support seeking backwards
	errno = EINVAL;
	return -1;
    }

    int i;

    // consume data from UART
    for(i = 0; i < curoff; i++)
    {
      int s, c;

      // wait for data to be available from the UART
      do
      {
        __PATMOS_UART_STATUS(s);
      } while((s & (__PATMOS_UART_DAV | __PATMOS_UART_PAE)) == 0);

      // reached EOF?
      if ((s & __PATMOS_UART_PAE) == 0)
      {
        // read the data from the UART.
        __PATMOS_UART_RD_DATA(c);
      }
      else
      {
        // signal EOF
        errno = EINVAL;
        return -1;
      }
    }

    // successful, update offset
    __patmos_stdin_offset += curoff;

    return __patmos_stdin_offset; 
  }

  // TODO: implement for simulator target
  errno  = EBADF;
  return -1;
}

