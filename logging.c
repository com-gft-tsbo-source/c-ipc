#include <stdio.h>
#include "logging.h"

void log_init(struct logsetup* ls, const char *module_name, const char *logfile_name, int loghandle){

    if (ls == NULL) return;

    strncpy(ls->module_name, module_name, sizeof(ls->module_name));
    ls->loghandle = loghandle;

    if (logfile_name == NULL){
        ls->logfile_name[0] = 0;
        ls->logfile == NULL;
    }
    else {
        ls->logfile = fopen(logfile_name, "a");
    }
}

void log_close(struct logsetup* ls) {

    if (ls == NULL) return;

    if (ls->logfile != NULL) {
        fclose(ls->logfile);
    }
    ls->module_name[0] = 0;
    ls->loghandle = -1;
    ls->logfile_name[0] = 0;
    ls->logfile == NULL;
}

void log_print(struct logsetup *ls, const char *fmt, ...) {

    if (ls == NULL) return;

    char message[ 4 * 1024 + 1];
    size_t len = 0;

    len = snprintf(message, sizeof(message), "[%s] ", ls->module_name);
    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf( message + len, sizeof(message) - len, fmt, ap);
    va_end(ap);

    if (ls->logfile != NULL) {
        fwrite(message, len, sizeof(char), ls->logfile);
        fflush(ls->logfile);
    }

    if (ls->loghandle > 0) {
        write(ls->loghandle, message, len);
        fsync(ls->loghandle);
    }

}
