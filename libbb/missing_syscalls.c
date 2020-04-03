/*
 * Copyright 2012, Denys Vlasenko
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
//kbuild:lib-y += missing_syscalls.o

#include "libbb.h"

#ifndef __NR_shmget
#define __NR_shmget 29
#endif

#ifndef __NR_shmat
#define __NR_shmat 30
#endif

#ifndef __NR_shmctl
#define __NR_shmctl 31
#endif

#ifndef __NR_semget
#define __NR_semget 64
#endif

#ifndef __NR_semop
#define __NR_semop 65
#endif

#ifndef __NR_semctl
#define __NR_semctl 66
#endif

#ifndef __NR_shmdt
#define __NR_shmdt 67
#endif

#ifndef __NR_msgget
#define __NR_msgget 68
#endif

#ifndef __NR_msgsnd
#define __NR_msgsnd 69
#endif

#ifndef __NR_msgrcv
#define __NR_msgrcv 70
#endif

#ifndef __NR_msgctl
#define __NR_msgctl 71
#endif

#if defined(ANDROID) || defined(__ANDROID__)
/*# include <linux/timex.h> - for struct timex, but may collide with <time.h> */
# include <sys/syscall.h>

int pivot_root(const char *new_root, const char *put_old)
{
	return syscall(__NR_pivot_root, new_root, put_old);
}

int shmget(key_t key, size_t size, int shmflg)
{
  return syscall(__NR_shmget, key, size, shmflg);
}

int shmdt(const void *shmaddr)
{
  return syscall(__NR_shmdt, shmaddr);
}

void *shmat(int shmid, const void *shmaddr, int shmflg)
{
  return (void *)syscall(__NR_shmat, shmid, shmaddr, shmflg);
}

int msgget(key_t key, int msgflg)
{
  return syscall(__NR_msgget, key, msgflg);
}

int semget(key_t key, int nsems, int semflg)
{
  return syscall(__NR_semget, key, nsems, semflg);
}

struct msqid_ds; /* #include <linux/msg.h> */
int msgctl(int msqid, int cmd, struct msqid_ds *buf)
{
  return syscall(__NR_msgctl, msqid, cmd, buf);
}

struct shmid_ds; /* #include <linux/shm.h> */
// NOTE: IPC_INFO takes a struct shminfo64
int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
  return syscall(__NR_shmctl, shmid, cmd, buf);
}

struct sembuf; /* #include <linux/sem.h> */
int semop(int semid, struct sembuf *sops, unsigned nsops)
{
  return syscall(__NR_semop, semid, sops, nsops);
}

# if __ANDROID_API__ < 21
int tcdrain(int fd)
{
	return ioctl(fd, TCSBRK, 1);
}
# endif
#endif
