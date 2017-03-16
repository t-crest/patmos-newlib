// Copyright 2012 TU Wien, Author: Stefan Hepp
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


