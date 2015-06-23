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

#include "patmos.h"


#undef errno
extern int  errno;


static inline unsigned long long _clock(void) {
    unsigned clo, chi;

    _iodev_ptr_t hi_clock = (_iodev_ptr_t)(__PATMOS_TIMER_BASE + 0x0);
    _iodev_ptr_t lo_clock = (_iodev_ptr_t)(__PATMOS_TIMER_BASE + 0x4);

    // Order is important here
    clo = *lo_clock;
    chi = *hi_clock;

    return (((unsigned long long) chi) << 32) | clo;
}

static inline unsigned long long _usecs(void) {
    unsigned ulo, uhi;

    _iodev_ptr_t hi_usec = (_iodev_ptr_t)(__PATMOS_TIMER_BASE + 0x8);
    _iodev_ptr_t lo_usec = (_iodev_ptr_t)(__PATMOS_TIMER_BASE + 0xc);

    // Order is important here
    ulo = *lo_usec;
    uhi = *hi_usec;

    return (((unsigned long long) uhi) << 32) | ulo;
}


//******************************************************************************
/// nanosleep - sleep for a given number of nanoseconds.
///
/// This implementation uses the µs clock source, so the minimal resolution is µs.
///
int nanosleep(const struct timespec *req, struct timespec *rem)
{
    unsigned long long wait_usec = req->tv_sec * 1000000 + (req->tv_nsec + 999) / 1000;

    unsigned long long end_ts = _usecs() + wait_usec;

    while (_usecs() < end_ts) { 
	// busy-wait.
    }

    // we do not support interrupts, so no need to update *rem.
    
    return 0;
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
    // TODO we should make sure that this is in terms of CLOCKS_PER_SEC
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

    unsigned long long us = _usecs();

    tv->tv_usec = us;
    tv->tv_sec = us / 1000000;
    return 0;
}


