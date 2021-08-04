#ifndef __LOGGING_H

#define __LOGGING_H

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ipc_common.h"
#include "config.h"

struct logsetup {
    char module_name[MAX_MODULE_NAME];
    char logfile_name[PATH_MAX + 1];
    int loghandle;
    FILE* logfile;
};

void log_init(struct logsetup* ls, const char *module_name, const char *logfile_name, int loghandle);
void log_print(struct logsetup* ls, const char *fmt, ...);
void log_close(struct logsetup* ls);

#endif