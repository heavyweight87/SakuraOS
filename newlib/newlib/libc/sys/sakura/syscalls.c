/* note these headers are all provided by newlib - you don't need to provide them */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include "syscalls.h"

int write(int file, char *ptr, int len)
{
	syscall(SYSCALL_WRITE, file, (int)ptr, len, 15, 16);
	return 0;
}

int isatty(int file)
{
	return 1;
}

int fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int read(int file, char *ptr, int len)
{
    syscall(SYSCALL_READ, file, (int)ptr, len, 15, 16);
	return 0;
}

int open(const char *name, int flags, ...)
{
	syscall(SYSCALL_OPEN, (int)name, flags, 33, 44, 55);
	return 0;
}

int close(int file)
{
	syscall(SYSCALL_CLOSE, file, 0, 0, 0, 6);
	return 0;
}

int lseek(int file, int ptr, int dir)
{
}

caddr_t sbrk(int incr)
{
    syscall(SYSCALL_SRBK, incr, 0, 0, 0, 6);
	return 0;
}

void _exit()
{
    syscall(SYSCALL_EXIT, 0, 0, 0, 0, 6);
}

int getpid()
{
	return 0;
}

int kill(int pid, int sig)
{
    syscall(SYSCALL_KILL, pid, sig, 0, 0, 6);
	return 0;
}
