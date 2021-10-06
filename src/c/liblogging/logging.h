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
    int filedes_logfile;
    int logfile_needs_close;
};

void log_init(struct logsetup* ls, const char *module_name, const char *logfile_name, int loghandle);
void log_print(struct logsetup* ls, const char *fmt, ...);
void log_errno(struct logsetup* ls, int err, const char *fmt, ...);
void log_close(struct logsetup* ls);
void log_signal(struct logsetup* ls, const char* msg);

#define LOG(...) log_print(ls, ##__VA_ARGS__)
#define LOG_PRINT(...) log_print(ls, ##__VA_ARGS__)
#define LOG_ERRNO(...) log_errno(ls, errno, ##__VA_ARGS__)
#define LOG_INIT(...) log_init(ls, ##__VA_ARGS__)
#define LOG_CLOSE() log_close(ls)
#define LOG_SIGNAL(msg) log_signal(ls, msg)

#endif