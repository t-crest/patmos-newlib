/*
 * POSIX conditional variable (pthread_cond) implementation file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#include <pthread_cond.h>

#include <errno.h>

#include "patmos.h"

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
  return 0;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
  return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *restrict clock_id)
{
  *clock_id = attr->clock_id;
  return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
  attr->clock_id = clock_id;
  return 0;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *cond_attr)
{
  *cond = PTHREAD_COND_INITIALIZER;
  if(cond_attr != NULL)
    cond->attr = *cond_attr;
  return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
  return pthread_mutex_destroy(&cond->mutex);
}

static int _pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const int timed, const struct timespec *restrict abstime)
{  
  int ret = pthread_mutex_lock(&cond->mutex);
  if(ret != 0)
    return ret;

  int ticket = ++cond->waiting;

  ret = pthread_mutex_unlock(&cond->mutex);
  if(ret != 0)
    return ret;

  // Assuming that the mutex was locked before calling wait
  ret = pthread_mutex_unlock(mutex);
  if(ret != 0)
    return ret;

  int loop = 1;
  while(loop) {
    if(timed) {
      //struct timespec tp;
      //clock_gettime(cond->attr.clock_id, &tp);
      // We are missing clock_gettime. Temporary hack
      const unsigned long long curtime = get_cpu_usecs();
      const unsigned long long USECONDS_PER_SECOND = 1000000;
      const unsigned long long _abstime = ((abstime->tv_sec*USECONDS_PER_SECOND)+abstime->tv_nsec)*(CLOCKS_PER_SEC/USECONDS_PER_SECOND);

      if(_abstime >= curtime) {
        pthread_mutex_lock(mutex);
        return ETIMEDOUT;
      }
    }

    ret = pthread_mutex_lock(&cond->mutex);
    if(ret != 0)
      return ret;

    if(ticket == cond->current) {
      // Current thread was notified
      if(cond->broadcasting) {
        if(cond->current == cond->broadcast)
          // All broadcasted threads have been notified,
          // so stop broadcasting
          cond->broadcasting = 0;
        if(cond->waiting != cond->current)
          // More threads are still waiting for normal signals
          cond->current++;
      }
      loop = 0;
    }
    ret = pthread_mutex_unlock(&cond->mutex);
    if(ret != 0)
      return ret; 
  }
  return pthread_mutex_lock(mutex);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  return _pthread_cond_wait(cond, mutex, 0, NULL);
}

int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime)
{  
  return _pthread_cond_wait(cond, mutex, 1, abstime);
}

static int _pthread_cond_signal(pthread_cond_t *cond, int broadcast)
{
  int ret = pthread_mutex_lock(&cond->mutex);
  if(ret != 0)
    return ret;

  if(cond->waiting != cond->current) {
    if(broadcast) {
      // Some threads are enqueued so start a broadcast
      // that lasts until all currently waiting threads
      // have been notified
      cond->broadcasting = 1;
      cond->broadcast = cond->waiting;
      cond->current++;
    }
    else {
      if(!cond->broadcasting)
        // Not broadcasting, so simply notify the next thread
        cond->current++;
      else if(cond->broadcast != cond->waiting)
        // Broadcasting and more threads have been enqueued
        // since the previous broadcast
        cond->broadcast++;
    }
  }
  return pthread_mutex_unlock(&cond->mutex);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
  return _pthread_cond_signal(cond, 0);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
  return _pthread_cond_signal(cond, 1);
}
