#include <stdio.h>
#include <string.h>
#include "logging.h"


struct logsetup global_ls;

struct message_buffer {
    char message[ 4 * 1024 + 1];
    size_t len;
};

void _log_write(struct logsetup *ls, const struct message_buffer *mb);

void log_init(struct logsetup* ls, const char *module_name, const char *logfile_name, int loghandle){

    if (ls == NULL) return;

    struct message_buffer mb;
    int has_error = 0;
    int has_loghandle = 0;

    // -----------------------------------------------------------------------
    // basic initialization
    
    memset(ls, sizeof(*ls), 0);
    strncpy(ls->module_name, module_name, sizeof(ls->module_name));

    // -----------------------------------------------------------------------
    // initialise the loghandle

    if (loghandle <= 0) {

        ls->loghandle = -1;

    } else {

        ls->loghandle = loghandle;
        has_loghandle = 1;

    }

    // -----------------------------------------------------------------------
    // initialize the logfile

    if (logfile_name == NULL || logfile_name[0] == 0){

        ls->logfile_name[0] = 0;
        ls->logfile == NULL;
        ls->filedes_logfile = -1;

    } else {

        if (logfile_name[0] == '-' && logfile_name[1] == 0) {

            ls->logfile = stdout;
            ls->logfile_needs_close = 0;

        } else {

            ls->logfile = fopen(logfile_name, "a");
            ls->logfile_needs_close = 0;

        }

        if (ls->logfile == NULL){

            const char* error_message = strerror(errno);
            mb.len = snprintf(mb.message, sizeof(mb.message), "[%s] ", ls->module_name);
            mb.len += snprintf(mb.message + mb.len, sizeof(mb.message) - mb.len, "%s: ", error_message);
            mb.len += snprintf( mb.message + mb.len, sizeof(mb.message) - mb.len, "Failed to open logfile '%s'.\n", logfile_name);
            has_error = 1;
        }
        else {

            ls->filedes_logfile = fileno(ls->logfile);
            
        }

    }

    // -----------------------------------------------------------------------
    // terminate on errors

    if (has_error){

        _log_write(ls, &mb);

        if (! has_loghandle) fprintf(stderr, mb.message);

        exit(1);
    }
}

void log_close(struct logsetup* ls) {

    if (ls == NULL) return;

    if (ls->logfile != NULL && ls->logfile_needs_close) {

        fclose(ls->logfile);

    }

    ls->module_name[0] = 0;
    ls->loghandle = -1;
    ls->logfile_name[0] = 0;
    ls->logfile == NULL;
}

void _log_fmt(struct logsetup *ls, struct message_buffer *mb, const char* fmt, va_list ap) {

    if (ls == NULL) return;

    mb->len = snprintf(mb->message, sizeof(mb->message), "[%s] ", ls->module_name);
    mb->len += vsnprintf( mb->message + mb->len, sizeof(mb->message) - mb->len, fmt, ap);
}

void _log_fmt_errno(struct logsetup *ls, struct message_buffer *mb, int err, const char* fmt, va_list ap) {

    if (ls == NULL) return;

    const char* error_message = strerror(err);

    mb->len = snprintf(mb->message, sizeof(mb->message), "[%s] ", ls->module_name);
    mb->len += snprintf(mb->message + mb->len, sizeof(mb->message) - mb->len, "%s: ", error_message);
    mb->len += vsnprintf( mb->message + mb->len, sizeof(mb->message) - mb->len, fmt, ap);
}

void _log_write(struct logsetup *ls, const struct message_buffer *mb) {

    if (ls == NULL) return;

    if (ls->logfile != NULL) {

        fwrite(mb->message, mb->len, sizeof(char), ls->logfile);
        fflush(ls->logfile);

    }

    if (ls->loghandle > 0) {

        write(ls->loghandle, mb->message, mb->len);
        fsync(ls->loghandle);

    }

}

void log_print(struct logsetup *ls, const char *fmt, ...) {

    if (ls == NULL) return;

    struct message_buffer mb;
    va_list ap;
    va_start(ap, fmt);
    _log_fmt(ls, &mb, fmt, ap);
    va_end(ap);
    _log_write(ls, &mb);

}

void log_errno(struct logsetup* ls, int err, const char *fmt, ...){

    if (ls == NULL) return;

    struct message_buffer mb;
    va_list ap;
    va_start(ap, fmt);
    _log_fmt_errno(ls, &mb, err, fmt, ap);
    va_end(ap);
    _log_write(ls, &mb);
}

void log_signal(struct logsetup *ls, const char* msg) {

    if (ls == NULL) return;
    if (msg == NULL) return;
    if (*msg == '\0') return;

    if (ls->filedes_logfile > 0) {

        write(ls->filedes_logfile, msg, strlen(msg));
        fsync(ls->filedes_logfile);

    }

    if (ls->loghandle > 0) {

        write(ls->loghandle, msg, strlen(msg));
        fsync(ls->loghandle);

    }

}

