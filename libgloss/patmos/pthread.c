/*
 * POSIX thread (pthread) implementation file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#include <pthread.h>

#include <errno.h>

#include "patmos.h"

// Max core count
static pthread_t * threads[64];
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void _start(void * arg)
{
  int id = (int) arg;
  // We invalidate the data cache upon thread start
  inval_dcache();
  void * retval = threads[id]->start_routine(threads[id]->arg);
  boot_info->slave[id].return_val = (int) retval;
  boot_info->slave[id].status = STATUS_RETURN;
}

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg)
{
  int id = get_cpuid();
  int cnt = get_cpucnt();
  if(pthread_mutex_lock(&lock) != 0)
    return EAGAIN;
  for(int i = 1; i < cnt; i++)
  {
    if(threads[i] == NULL)
    {
      if(i == id)
        // The current core is not registered as running a thread,
        // yet it is calling pthread_create. Should not happen, but
        // if it does, we return an error
        return -1;

      threads[i] = thread;
      pthread_mutex_unlock(&lock);
      thread->start_routine = start_routine;
      thread->arg = arg;
      thread->cpuid = i;
      

      if(boot_info->slave[i].status == STATUS_INITDONE ||
          boot_info->slave[i].funcpoint != NULL)
        // Core i is registered as available, but is not ready
        // to start a thread. Should not happen, but
        // if it does, we return an error
        return -2;
        
      boot_info->slave[i].param = (void *)i;
      boot_info->slave[i].funcpoint = (funcpoint_t)_start;
      return 0;
    }
  }
  pthread_mutex_unlock(&lock);
  return EAGAIN;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
  int id = thread.cpuid;
  unsigned long long time;
  while(boot_info->slave[id].status != STATUS_RETURN) {
    time = get_cpu_usecs();
    while(get_cpu_usecs() < time+10) {
      // Wait for 10 microseconds before checking again
    }
  }
  inval_dcache();
  if(value_ptr != NULL)
    *value_ptr = (void *) boot_info->slave[id].return_val;
  boot_info->slave[id].funcpoint = NULL;
  asm volatile ("" : : : "memory");
  threads[id] = NULL;
  return 0;
}
