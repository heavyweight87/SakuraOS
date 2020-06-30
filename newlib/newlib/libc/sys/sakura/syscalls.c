/* note these headers are all provided by newlib - you don't need to provide them */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
 
/*void _exit();
int close(int file);
char **environ; // pointer to array of char * strings that define the current environment variables 
int execve(char *name, char **argv, char **env);
int fork();
int getpid();
int kill(int pid, int sig);
int link(char *old, char *new);


int stat(const char *file, struct stat *st);
clock_t times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
int gettimeofday(struct timeval *p, struct timezone *z);*/

int write(int file, char *ptr, int len)
{
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
