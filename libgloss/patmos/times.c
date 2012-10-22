// Copyright 2012 Stefan Hepp
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
#include <sys/times.h>
#include <time.h>


#define CLOCKS_PER_USEC ((CLOCKS_PER_SEC) / 1000000)

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
///
/// Used by _times_r(), times() and clock().
/// Note that clock() returns clock ticks in terms of CLOCKS_PER_SEC, while times() 
/// returns clock ticks in terms of sysconf(_SC_CLK_TCK), but in newlib clock() uses
/// _times_r().
///
clock_t _times (struct tms* buf)
{
    unsigned long long ticks;
    ticks = _clock();
    buf->tms_utime = ticks;	// user time
    buf->tms_stime = 0;		// system time
    buf->tms_cutime = 0;	// childs user time
    buf->tms_cstime = 0;	// childs sys time
    return ticks;
}


/// _gettimeofday -- implement in terms of time.
///
/// Used by gettimeofday(), time().
///
int _gettimeofday (struct timeval *tv, void *tzvp)
{
    struct timezone *tz = tzvp;
    if (tz) {
	tz->tz_minuteswest = tz->tz_dsttime = 0;
    }

    unsigned long long c = _clock();

    tv->tv_usec = c / CLOCKS_PER_USEC;
    tv->tv_sec = c / CLOCKS_PER_SEC;
    return 0;
}


