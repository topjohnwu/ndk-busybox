/*
 * Copyright 2012, Denys Vlasenko
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
//kbuild:lib-y += missing_syscalls.o

#include "libbb.h"

#if defined(ANDROID) || defined(__ANDROID__)
/*# include <linux/timex.h> - for struct timex, but may collide with <time.h> */
# include <sys/syscall.h>
pid_t getsid(pid_t pid)
{
	return syscall(__NR_getsid, pid);
}

int sethostname(const char *name, size_t len)
{
	return syscall(__NR_sethostname, name, len);
}

struct timex;
int adjtimex(struct timex *buf)
{
	return syscall(__NR_adjtimex, buf);
}

int pivot_root(const char *new_root, const char *put_old)
{
	return syscall(__NR_pivot_root, new_root, put_old);
}

int shmget(key_t key, size_t size, int shmflg)
{
  return syscall(__NR_shmget, key, size, shmflg);
}

int msgget(key_t key, int msgflg)
{
  return syscall(__NR_msgget, key, msgflg);
}

int semget(key_t key, int nsems, int semflg)
{
  return syscall(__NR_semget, key, nsems, semflg);
}

# if __ANDROID_API__ < 21
int tcdrain(int fd)
{
	return ioctl(fd, TCSBRK, 1);
}
# endif
#endif
