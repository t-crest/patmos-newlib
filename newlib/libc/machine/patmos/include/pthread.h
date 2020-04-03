/*
 * POSIX thread (pthread) definition file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#ifndef _PTHREAD_H
#define _PTHREAD_H 1

#include <pthread_mutex.h>
#include <pthread_cond.h>

typedef struct {
  void *(*start_routine)(void*);
  void *arg;
  int cpuid;
} pthread_t;

typedef struct {
} pthread_attr_t;

extern int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg);
extern int pthread_join(pthread_t thread, void **value_ptr);

#endif /* _PTHREAD_H */
