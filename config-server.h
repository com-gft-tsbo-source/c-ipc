#ifdef __CONFIG_SERVER_H
#else
#define __CONFIG_SERVER_H

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <linux/limits.h>

static char doc[] = "Your program description.";
static char args_doc[] = "[SERVER ID]";

static struct argp_option options[] = { 
    { "logfile", 'l', "logfile path", 0, "Path to the logfile."},
    { "server-queue", 's', "queue name", 0, "Central communications queue."},
    { "no-daemon", 'n', 0, 0, "Don't run as a daemon."},
    { 0 } 
};

struct arguments {
    char module_id[MAX_MODULE_NAME + 1];
    char server_queue[MAX_QUEUE_NAME + 1];
    char logfile[PATH_MAX + 1];
    int no_daemon;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {

        case 'l': 

            strncpy(arguments->logfile, arg, sizeof(arguments->logfile));
            arguments->logfile[sizeof(arguments->logfile) - 1] = 0;
            break;

        case 's': 

            strncpy(arguments->server_queue, arg, sizeof(arguments->server_queue));
            arguments->server_queue[sizeof(arguments->server_queue) - 1] = 0;
            break;

        case 'n': 

            arguments->no_daemon = 1;
            break;

        case ARGP_KEY_ARG: 

            if (state->arg_num >= 1)
            {
                argp_usage(state);
            }

            strncpy(arguments->module_id, arg, sizeof(arguments->module_id));
            arguments->module_id[sizeof(arguments->module_id) - 1] = 0;
            break;

        case ARGP_KEY_END:

            if (state->arg_num < 1)
            {
                fprintf(stderr, "Error: Too few arguments!\n");
                argp_usage(state);
            }

            break;

        default: 

            return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc};

#endif