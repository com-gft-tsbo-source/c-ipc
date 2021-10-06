#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "lockfile.h"
#include "logging.h"

extern struct logsetup *ls;

int create_pidfile(const char* pidfile)
{
    int fd;
    char buf[16];
    size_t len=0;

    LOG("Using pidfile '%s'.\n", pidfile);
    fd = open(pidfile, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    if (fd < 0) {
        LOG_ERRNO("Can′t open pidfile '%s'.\n", pidfile);
        exit(EXIT_FAILURE);
    }

    if (lockfile(fd) < 0) {
        LOG_ERRNO("Can′t lock pidfile '%s'.\n", pidfile);
        exit(EXIT_FAILURE);
    }

    ftruncate(fd, 0);
    len = snprintf(buf, sizeof(buf), "%ld", (long)getpid());
    write(fd, buf, len);
    fsync(fd);
    // keep fd open, so lsof can check for owner
    return(0);
}
