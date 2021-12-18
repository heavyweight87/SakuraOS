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

int _write(int file, char *ptr, int len)
{
	return write(file, ptr, len);
}

int isatty(int file)
{
	syscall(50, 0, 0, 33, 44, 55);
	return 1;
}

int _isatty(int file)
{
	return isatty(file);
}

int fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	syscall(50, 0, 0, 33, 44, 55);
	return 0;
}

int _fstat(int file, struct stat *st)
{
	return fstat(file, st);
}

int read(int file, char *ptr, int len)
{
    syscall(SYSCALL_READ, file, (int)ptr, len, 15, 16);
	return 0;
}

int _read(int file, char *ptr, int len)
{
	return read(file, ptr, len);
}

int open(const char *name, int flags, ...)
{
	syscall(SYSCALL_OPEN, (int)name, flags, 33, 44, 55);
	return 0;
}

int _open(const char *name, int flags)
{
	return open(name, flags);
}

int close(int file)
{
	syscall(SYSCALL_CLOSE, file, 0, 0, 0, 6);
	return 0;
}

int _close(int file)
{
	return close(file);
}

int lseek(int file, int ptr, int dir)
{
	syscall(50, 0, 0, 33, 44, 55);
}

int _lseek(int file, int ptr, int dir)
{
	return lseek(file, ptr, dir);
}

caddr_t sbrk(int incr)
{
    syscall(SYSCALL_SBRK, incr, 0, 0, 0, 7);
	return 0;
}

caddr_t _sbrk(int incr)
{
    return sbrk(incr);
}

void _exit()
{
    syscall(SYSCALL_EXIT, 0, 0, 0, 0, 6);
}

int getpid()
{
	syscall(50, 0, 0, 33, 44, 55);
	return 0;
}

int _getpid()
{
	return getpid();
}

int kill(int pid, int sig)
{
    syscall(SYSCALL_KILL, pid, sig, 0, 0, 6);
	return 0;
}

int _kill(int pid, int sig)
{
    return kill(pid, sig);
}
