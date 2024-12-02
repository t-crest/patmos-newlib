/*
 * POSIX thread (pthread) implementation file.
 * 
 * Author: Torur Biskopsto Strom (torur.strom@gmail.com)
 */

#include <pthread.h>

#include <errno.h>
#include <machine/patmos.h>
#include "patmos.h"

static inline int context_save(void) {
  asm volatile ("" : : : "memory");
  int shadowstack_ptr;
  asm volatile (
    "sub $r31 = $r31, 200;"
    "swc [$r31 + 0] = $r31;"
    "swc [$r31 + 1] = $r1;"
    "swc [$r31 + 2] = $r2;"
    "swc [$r31 + 3] = $r3;"
    "swc [$r31 + 4] = $r4;"
    "swc [$r31 + 5] = $r5;"
    "swc [$r31 + 6] = $r6;"
    "swc [$r31 + 7] = $r7;"
    "swc [$r31 + 8] = $r8;"
    "swc [$r31 + 9] = $r9;"
    "swc [$r31 + 10] = $r10;"
    "swc [$r31 + 11] = $r11;"
    "swc [$r31 + 12] = $r12;"
    "swc [$r31 + 13] = $r13;"
    "swc [$r31 + 14] = $r14;"
    "swc [$r31 + 15] = $r15;"
    "swc [$r31 + 16] = $r16;"
    "swc [$r31 + 17] = $r17;"
    "swc [$r31 + 18] = $r18;"
    "swc [$r31 + 19] = $r19;"
    "swc [$r31 + 20] = $r20;"
    "swc [$r31 + 21] = $r21;"
    "swc [$r31 + 22] = $r22;"
    "swc [$r31 + 23] = $r23;"
    "swc [$r31 + 24] = $r24;"
    "swc [$r31 + 25] = $r25;"
    "swc [$r31 + 26] = $r26;"
    "swc [$r31 + 27] = $r27;"
    "swc [$r31 + 28] = $r28;"
    "swc [$r31 + 29] = $r29;"
    "swc [$r31 + 30] = $r30;"
    "mfs $r29 = $s1;"
    "swc [$r31 + 31] = $r29;"
    "mfs $r29 = $s2;"
    "swc [$r31 + 32] = $r29;"
    "mfs $r29 = $s3;"
    "swc [$r31 + 33] = $r29;"
    "mfs $r29 = $s4;"
    "swc [$r31 + 34] = $r29;"
    "mfs $r28 = $s5;"
    "mfs $r29 = $s6;"
    "sub $r28 = $r28, $r29;"
    "sspill $r28;"
    "swc [$r31 + 35] = $r28;"
    "swc [$r31 + 36] = $r29;"
    "mfs $r29 = $s7;"
    "swc [$r31 + 37] = $r29;"
    "mfs $r29 = $s8;"
    "swc [$r31 + 38] = $r29;"
    "mfs $r29 = $s9;"
    "swc [$r31 + 39] = $r29;"
    "mfs $r29 = $s10;"
    "swc [$r31 + 40] = $r29;"
    "mfs $r29 = $s11;"
    "swc [$r31 + 41] = $r29;"
    "mfs $r29 = $s12;"
    "swc [$r31 + 42] = $r29;"
    "mfs $r29 = $s13;"
    "swc [$r31 + 43] = $r29;"
    "mfs $r29 = $s14;"
    "swc [$r31 + 44] = $r29;"
    "mfs $r29 = $s15;"
    "swc [$r31 + 45] = $r29;"
    "mfs $r29 = $s0;"
    "swc [$r31 + 46] = $r29;"
    "mov %0 = $r31;"
    : "=r"(shadowstack_ptr) : :
    "r28", "r29", "r31");
  asm volatile ("" : : : "memory");
  return shadowstack_ptr;
}

static inline void context_load(int shadowstack_ptr) {
  asm volatile ("" : : : "memory");
  asm volatile (
    "mov  $r31 = %0;"
    "lwc $r16 = [$r31 + 46];"
    "lwc $r15 = [$r31 + 45];"
    "lwc $r14 = [$r31 + 44];"
    "lwc $r13 = [$r31 + 43];"
    "lwc $r12 = [$r31 + 42];"
    "lwc $r11 = [$r31 + 41];"
    "lwc $r10 = [$r31 + 40];"
    "lwc $r9 = [$r31 + 39];"
    "lwc $r8 = [$r31 + 38];"
    "lwc $r7 = [$r31 + 37];"
    "lwc $r6 = [$r31 + 36];"
    "lwc $r5 = [$r31 + 35];"
    "lwc $r4 = [$r31 + 34];"
    "lwc $r3 = [$r31 + 33];"
    "lwc $r2 = [$r31 + 32];"
    "lwc $r1 = [$r31 + 31];"
    "mts $s0 = $r16;"
    "mts $s15 = $r15;"
    "mts $s14 = $r14;"
    "mts $s13 = $r13;"
    "mts $s12 = $r12;"
    "mts $s11 = $r11;"
    "mts $s10 = $r10;"
    "mts $s9 = $r9;"
    "mts $s8 = $r8;"
    "mts $s7 = $r7;"
    "mts $s6 = $r6;"
    "mts $s5 = $r6;" // Use the same value for both pointers
    "mts $s4 = $r4;"
    "mts $s3 = $r3;"
    "mts $s2 = $r2;"
    "mts $s1 = $r1;"
    "sens $r5;"
    "lwc $r30 = [$r31 + 30];"
    "lwc $r29 = [$r31 + 29];"
    "lwc $r28 = [$r31 + 28];"
    "lwc $r27 = [$r31 + 27];"
    "lwc $r26 = [$r31 + 26];"
    "lwc $r25 = [$r31 + 25];"
    "lwc $r24 = [$r31 + 24];"
    "lwc $r23 = [$r31 + 23];"
    "lwc $r22 = [$r31 + 22];"
    "lwc $r21 = [$r31 + 21];"
    "lwc $r20 = [$r31 + 20];"
    "lwc $r19 = [$r31 + 19];"
    "lwc $r18 = [$r31 + 18];"
    "lwc $r17 = [$r31 + 17];"
    "lwc $r16 = [$r31 + 16];"
    "lwc $r15 = [$r31 + 15];"
    "lwc $r14 = [$r31 + 14];"
    "lwc $r13 = [$r31 + 13];"
    "lwc $r12 = [$r31 + 12];"
    "lwc $r11 = [$r31 + 11];"
    "lwc $r10 = [$r31 + 10];"
    "lwc $r9 = [$r31 + 9];"
    "lwc $r8 = [$r31 + 8];"
    "lwc $r7 = [$r31 + 7];"
    "lwc $r6 = [$r31 + 6];"
    "lwc $r5 = [$r31 + 5];"
    "lwc $r4 = [$r31 + 4];"
    "lwc $r3 = [$r31 + 3];"
    "ret;"
    "lwc $r2 = [$r31 + 2];"
    "lwc $r1 = [$r31 + 1];"
    "add $r31 = $r31, 200;"
    : : "r" (shadowstack_ptr) :
    "r1", "r2", "r3",
    "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11",
    "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19",
    "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "r27",
    "r28", "r29", "r30", "r31",
    "s0", "s1", "s2", "s3",
    "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11",
    "s12", "s13", "s14", "s15");
  asm volatile ("" : : : "memory");
}

// Max core count
static pthread_t * threads[64];
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void _context_save(pthread_t *restrict thread) __attribute__((noinline));
static void _context_save(pthread_t *restrict thread)
{
  thread->shadowstack_ptr = context_save();
}

static void * _pthread_start(void * arg)
{
  // We invalidate the data cache upon thread start
  inval_dcache();
  int id = get_cpuid();
  pthread_t * thread = threads[id];
  _context_save(thread);
  if(!thread->exit_called) {
    void * retval = thread->start_routine(thread->arg);
    pthread_exit(retval);
  }
  return NULL;
}

void pthread_exit(void *value_ptr)
{
  int id = get_cpuid();
  pthread_t * thread = threads[id];
  thread->value_ptr = value_ptr;
  asm volatile ("" : : : "memory");
  thread->exit_called = 1;
  context_load(thread->shadowstack_ptr);
}

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void*), void *restrict arg)
{
  int id = get_cpuid();
  int cnt = get_cpucnt();
  thread->start_routine = start_routine;
  thread->exit_called = 0;
  thread->arg = arg;
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

      thread->cpuid = i;
      threads[i] = thread;
      pthread_mutex_unlock(&lock);
      boot_info->slave[i].funcpoint = (funcpoint_t)_pthread_start;
      return 0;
    }
  }
  pthread_mutex_unlock(&lock);
  return EAGAIN;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
  int id = thread.cpuid;
  _UNCACHED pthread_t * _thread = (_UNCACHED pthread_t *)threads[id];

  while(!_thread->exit_called) {
    unsigned long long time = get_cpu_usecs();
    while(get_cpu_usecs() < time+10) {
      // Wait for 10 microseconds before checking again
    }
  }
  
  if(value_ptr != NULL)
    *value_ptr = _thread->value_ptr;
  boot_info->slave[id].funcpoint = NULL;
  inval_dcache();
  asm volatile ("" : : : "memory");
  threads[id] = NULL;
  return 0;
}
