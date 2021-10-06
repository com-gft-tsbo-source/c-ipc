#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>
#include <signal.h>

#include "daemon.h"
#include "logging.h"

extern  struct logsetup global_ls;
struct logsetup *ls = &global_ls;

// ###########################################################################
// ###########################################################################
// Signal handling
// ###########################################################################
// ###########################################################################

static void do_sigterm (int sig_nr) {
    LOG_SIGNAL("Terminating for SIGTERM.\n");
    exit(1);
}

static void do_sigint (int sig_nr) {
    LOG_SIGNAL("Terminating for SIGINT.\n");
    exit(1);
}

// ###########################################################################
// ###########################################################################
// MAIN
// ###########################################################################
// ###########################################################################


int main(int argc, char **argv)
{
    const char* logfile = argv[1];
    int exit_status = 0;
    int fd = -1;

    handle_signal(SIGHUP, SIG_IGN);
    handle_signal(SIGTERM, do_sigterm);
    handle_signal(SIGINT, do_sigint);
    LOG_INIT( "tail", NULL, fileno(stdout));

    while (fd == -1)
    {
        fd = open(logfile, O_RDONLY);

        if (fd >= 0) break;

        if (errno != ENOENT) {
            LOG_ERRNO("Can′t open logfile '%s'.\n", logfile);
            exit(1);
        }

        LOG_PRINT("Logfile '%s' not found, waiting ...\n", logfile);
        sleep(1);
    }

    struct stat stats;
    int status;

    status = fstat(fd, &stats);

    if (status) {
        LOG_ERRNO("Can′t fstat  logfile '%s'.\n", logfile);
        exit(1);
    }

    char BUFFER[10];
    off_t pos = 0;
    off_t end_seek = 0;

    while(1) {

        ssize_t b;

        b = read(fd, BUFFER, sizeof(BUFFER) - 1);
        end_seek = lseek(fd, 0, SEEK_CUR);

        if (b == 0){ 
            sleep(1);
            status = fstat(fd, &stats);
            if (stats.st_size < end_seek) lseek(fd, 0, SEEK_SET);
            continue;
        }

        BUFFER[b] = 0;
        printf("%s", BUFFER);
        fflush(stdout);

    }
    exit(exit_status);
}