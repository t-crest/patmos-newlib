/*
 * POSIX thread (pthread) definition file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#ifndef _PTHREAD_H
#define _PTHREAD_H 1

#include <pthread_cond.h>

typedef struct {
  void *(*start_routine)(void*);
  void * arg;
  void * value_ptr;
  int cpuid;
  int shadowstack_ptr;
  int exit_called;
} pthread_t;

typedef struct {
} pthread_attr_t;

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg);
int pthread_join(pthread_t thread, void **value_ptr);
void pthread_exit(void *value_ptr);

#endif /* _PTHREAD_H */
