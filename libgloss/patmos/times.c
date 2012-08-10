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
    
#include <errno.h>
#include <sys/time.h>

#undef errno
extern int  errno;

//******************************************************************************
/// _times - get timing information.
int _times(int nbytes)
{
  // TODO: implement for simulator target
  errno  = EACCES;
  return -1;
}


/* _gettimeofday -- implement in terms of time.  */
int _gettimeofday (struct timeval *tv, void *tzvp)
{
    struct timezone *tz = tzvp;
    if (tz)
	tz->tz_minuteswest = tz->tz_dsttime = 0;

    // TODO read out cycle counter, get usecs and secs since boot
    tv->tv_usec = 0;
    tv->tv_sec = _times (0);
    return 0;
}


