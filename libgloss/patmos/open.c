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
    
#include <errno.h>
#include <stdarg.h>

#undef errno
extern int  errno;

//******************************************************************************
/// patmos-plug: open: Implements the default `_open` implementation used when
/// no specific implementation is provided at link time.
/// Called if `patmosplug_open(const char *, int, int)` is not defined.
int _patmosplug_open(const char *name, int flags, int mode) {
  errno = ENOSYS;
  return -1;
}

/// patmosplug_open: Alternative, patmos-specific `_open` implementation that
/// can be provided at program link time.
/// If not provided, will default to calling `_patmosplug_open`.
int patmosplug_open(const char *name, int flags, int mode)
    __attribute__((weak, alias("_patmosplug_open")));

//******************************************************************************
/// _open - open a file.
/// 24-12-2020: Changed to varargs, as newlibc will be compiled expecting a
/// platform specific `_open` function with varargs.
/// See `newlib/libc/include/sys/_default_fcntl.h`
int _open(const char *name, int flags, ...)
{
  va_list args;
  int mode, errorcode;
  va_start(args, flags);

  mode = va_arg(args, int);
  
  /* execute patmos-plugin implementation */
  errorcode = patmosplug_open(name, flags, mode);

  va_end(args);
  return errorcode;
}
