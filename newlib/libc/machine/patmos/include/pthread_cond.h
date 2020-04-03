/*
 * POSIX conditional variable (pthread_cond) definition file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#ifndef _PTHREAD_COND_H_
#define _PTHREAD_COND_H_

#include <pthread_mutex.h>

#include <time.h>

typedef struct
{
  clockid_t clock_id;
} pthread_condattr_t;

typedef struct
{
  int waiting;
  int current;
  int broadcasting;
  int broadcast;
  pthread_mutex_t mutex;
  pthread_condattr_t attr;
} pthread_cond_t;

/* This is used to statically initialize a pthread_cond_t. Example:

    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 */

#define PTHREAD_COND_INITIALIZER  ((pthread_cond_t) {0, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, {0}})

int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t *restrict clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr, int *restrict pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

#endif /* _PTHREAD_COND_H_ */
