/* note these headers are all provided by newlib - you don't need to provide them */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include "Syscalls.h"

int write(int file, char *ptr, int len)
{
	syscall(SYSCALL_WRITE, file, (int)ptr, len, 15, 16);
	return 0;
}

int isatty(int file)
{
	return 0;
}

int fstat(int file, struct stat *st)
{
	return 0;
}

int read(int file, char *ptr, int len)
{
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
	return -1;
}

void _exit()
{
}

int getpid()
{
	return 0;
}

int kill(int pid, int sig)
{
	return 0;
}
