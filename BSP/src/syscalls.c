#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

register char * stack_ptr asm("sp");

char *__env[1] = { 0 };
char **environ = __env;

void initialise_monitor_handles()
{
}

int _getpid(void)
{
}

int _kill(int pid, int sig)
{
}

void _exit (int status)
{
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
}

int _close(int file)
{
}

int _fstat(int file, struct stat *st)
{
}

int _isatty(int file)
{
}

int _lseek(int file, int ptr, int dir)
{
}

int _open(char *path, int flags, ...)
{
}

int _wait(int *status)
{
}

int _unlink(char *name)
{
}

int _times(struct tms *buf)
{
}

int _stat(char *file, struct stat *st)
{
}

int _link(char *old, char *new)
{
}

int _fork(void)
{
}

int _execve(char *name, char **argv, char **env)
{
}
