/* Reentrant versions of fcntl system call.  This implementation just
   calls the fcntl system call.  */

#include <reent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <_syslist.h>

/* Some targets provides their own versions of these functions.  Those
   targets should define REENTRANT_SYSCALLS_PROVIDED in TARGET_CFLAGS.  */

#ifdef _REENT_ONLY
#ifndef REENTRANT_SYSCALLS_PROVIDED
#define REENTRANT_SYSCALLS_PROVIDED
#endif
#endif

