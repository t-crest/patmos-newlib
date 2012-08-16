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

// TODO define CLOCKS_PER_SEC in sys/time, define clock_t as long long

#define _CLOCKS_PER_SEC_ 100000000

#define _CLOCKS_PER_USEC_ ((_CLOCKS_PER_SEC_) / 1000000)

#undef errno
extern int  errno;

static inline unsigned long long _clock(void) {
    unsigned clo, chi;

    asm volatile ( "mfs %0 = $s7 ; mfs %1 = $s8"
	 : "=r" (chi), "=r" (clo) : );

    return (((unsigned long long) chi) << 32) | clo;
}


//******************************************************************************
/// _times - get timing information.
int _times(int nbytes)
{
  return _clock() / _CLOCKS_PER_SEC_;
}


/* _gettimeofday -- implement in terms of time.  */
int _gettimeofday (struct timeval *tv, void *tzvp)
{
    struct timezone *tz = tzvp;
    if (tz)
	tz->tz_minuteswest = tz->tz_dsttime = 0;

    unsigned long long c = _clock();

    tv->tv_usec = c / _CLOCKS_PER_USEC_;
    tv->tv_sec = c / _CLOCKS_PER_SEC_;
    return 0;
}


