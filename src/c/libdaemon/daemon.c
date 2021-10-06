#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "logging.h"
#include "lockfile.h"

extern struct logsetup *ls;

// ###########################################################################
// ###########################################################################
// Daemonisation
// ###########################################################################
// ###########################################################################

void to_daemon(){
    int status;

    // Clear file creation rights mask
    umask(0);

    // become session leader and disconnect controlling TTY
    pid_t pid;
    pid = fork();

    if(pid < 0){
        LOG_ERRNO("First fork failed!.\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) exit(EXIT_SUCCESS);

    status = setsid();

    if (status < 0){
        LOG_ERRNO("Setsid failed!.\n");
        exit(EXIT_FAILURE);
    }

    // prevent open to allocate controlling TTYs
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);

    status = sigaction(SIGHUP, &sa, NULL);
    if (status < 0){
        LOG_ERRNO("Cannot ignore SIGHUP!.\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if(pid < 0){
        LOG_ERRNO("Second fork failed!.\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) exit(EXIT_SUCCESS);

    // Move current working dir to root, to allow unmounts
    status = chdir("/");
    if(pid < 0){
        LOG_ERRNO("Chdir / failed!.\n");
        exit(EXIT_FAILURE);
    }

    // Close open filedescriptors
    int i;

    for (i = sysconf(_SC_OPEN_MAX); i > 0; i--)
        close(i);

    // Attach stdin, stdout and stderr to /dev/null
    int fd0, fd1, fd2;
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

}

