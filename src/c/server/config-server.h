#ifdef __CONFIG_SERVER_H
#else
#define __CONFIG_SERVER_H

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <string.h>
#include <linux/limits.h>

struct arguments {
    char module_id[MAX_MODULE_NAME + 1];
    char server_queue[MAX_QUEUE_NAME + 1];
    char logfile[PATH_MAX + 1];
    char pidfile[PATH_MAX + 1];
    int no_daemon;
};

#endif