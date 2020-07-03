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
	syscall(SYSCALL_WRITE, 12, 13, 14, 15, 16);
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
	syscall(SYSCALL_OPEN, 11, 22, 33, 44, 55);
	return 0;
}

int close(int file)
{
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
