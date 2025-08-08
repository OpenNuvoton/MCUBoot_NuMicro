#if defined(__ARMCC_VERSION)
void exit()
{
    while(1);
}
#else
#include <errno.h>
#include <sys/stat.h>

extern int errno;

int __attribute__((weak))
_close(int fildes __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_fstat(int fildes __attribute__((unused)),
       struct stat* st __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_getpid(void)
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_isatty(int file __attribute__((unused)))
{
    errno = ENOSYS;
    return 0;
}

int __attribute__((weak))
_kill(int pid __attribute__((unused)), int sig __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}

int __attribute__((weak))
_lseek(int file __attribute__((unused)), int ptr __attribute__((unused)),
       int dir __attribute__((unused)))
{
    errno = ENOSYS;
    return -1;
}
#endif