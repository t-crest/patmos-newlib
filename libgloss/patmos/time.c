/*
 * POSIX time implementation file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#include <time.h>

#include <errno.h>
#include <machine/rtc.h>

inline clock_t clock(void) {
  return get_cpu_cycles();
}

struct timespec _clock_realtime_offset;

int clock_getres(clockid_t clock_id, struct timespec *res)
{
  switch (clock_id)
  {
    case CLOCK_REALTIME:
      res->tv_sec = 0;
      res->tv_nsec = 1000;
      return 0;
    default:
      return EINVAL;
  }
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
  unsigned long long us;
  switch (clock_id)
  {
    case CLOCK_REALTIME:
      us = get_cpu_usecs();
      us += (_clock_realtime_offset.tv_sec*1000000)+(_clock_realtime_offset.tv_nsec/1000);
      tp->tv_sec = us/1000000;
      us -= tp->tv_sec*1000000;
      tp->tv_nsec = us*1000;
      return 0;
    default:
      return EINVAL;
  }
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
  switch (clock_id)
  {
    case CLOCK_REALTIME:
      _clock_realtime_offset = *tp;
      return 0;
    default:
      return EINVAL;
  }
}
