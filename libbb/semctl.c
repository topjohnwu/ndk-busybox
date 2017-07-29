/* Copyright (C) 1995,1997,1998,2000,2003,2004,2006
        Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, August 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */
/* originally from glibc-2.14/sysdeps/unix/sysv/linux/semctl.c, modified */

// syscall used by syslogd, ipcrm, ipcs
//kbuild:lib-y += semctl.o

#include <sys/syscall.h>   /* For __NR_xxx definitions */
#include <stdarg.h>
#include <sys/sem.h>
#include "libbb.h"

#ifndef __NR_semctl
#define __NR_semctl 66
#endif

/* code from GLIBC */
int semctl(int semid, int semnum, int cmd, ...) {
  union semun arg;
  va_list ap;

  va_start (ap, cmd);

  /* Get the argument only if required.  */
  arg.buf = NULL;
  switch (cmd)
  {
    case SETVAL:        /* arg.val */
    case GETALL:        /* arg.array */
    case SETALL:
    case IPC_STAT:      /* arg.buf */
    case IPC_SET:
    case SEM_STAT:
    case IPC_INFO:      /* arg.__buf */
    case SEM_INFO:
      va_start (ap, cmd);
      arg = va_arg (ap, union semun);
      va_end (ap);
      break;
  }

  va_end (ap);

  return syscall(__NR_semctl, semid, semnum, cmd, arg);
}
