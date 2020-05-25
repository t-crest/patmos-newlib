/*
 * POSIX time definition file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#ifndef _TIME_H_
#define _TIME_H_ 1

#include <sys/types.h>

/* Get _CLOCKS_PER_SEC_ */
#include <machine/time.h>
#define CLOCKS_PER_SEC _CLOCKS_PER_SEC_

#define CLOCK_REALTIME (clockid_t)1
#define CLOCK_PROCESS_CPUTIME_ID (clockid_t)2
#define CLOCK_THREAD_CPUTIME_ID (clockid_t)3
#define CLOCK_MONOTONIC (clockid_t)4

#define TIMER_ABSTIME 4

struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

typedef struct __tzrule_struct
{
  char ch;
  int m;
  int n;
  int d;
  int s;
  time_t change;
  long offset; /* Match type of _timezone. */
} __tzrule_type;

typedef struct __tzinfo_struct
{
  int __tznorth;
  int __tzyear;
  __tzrule_type __tzrule[2];
} __tzinfo_type;

#define getdate_err (*__getdate_err())
int *_EXFUN(__getdate_err,(_VOID));

struct tm *	_EXFUN(getdate, (const char *));
/* getdate_err is set to one of the following values to indicate the error.
     1  the DATEMSK environment variable is null or undefined,
     2  the template file cannot be opened for reading,
     3  failed to get file status information,
     4  the template file is not a regular file,
     5  an error is encountered while reading the template file,
     6  memory allication failed (not enough memory available),
     7  there is no line in the template that matches the input,
     8  invalid input specification  */

/* getdate_r returns the error code as above */
int _EXFUN(getdate_r, (const char *, struct tm *));

/* defines for the opengroup specifications Derived from Issue 1 of the SVID.  */
extern __IMPORT long _timezone;
extern __IMPORT int _daylight;
extern __IMPORT char *_tzname[2];

int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_settime(clockid_t clock_id, const struct timespec *tp); 

#endif /* _TIME_H_ */

