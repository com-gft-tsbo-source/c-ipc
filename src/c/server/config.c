#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc_common.h"
#include "config.h"
#include "config-server.h"
#include "logging.h"
#include "daemon.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {

        case 'l': 

            strncpy(arguments->logfile, arg, sizeof(arguments->logfile));
            arguments->logfile[sizeof(arguments->logfile) - 1] = 0;
            break;

        case 'p': 

            strncpy(arguments->pidfile, arg, sizeof(arguments->pidfile));
            arguments->pidfile[sizeof(arguments->pidfile) - 1] = 0;
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
                fprintf(stderr, "Error: Too many arguments!\n");
                argp_usage(state);
            }

            strncpy(arguments->module_id, arg, sizeof(arguments->module_id));
            arguments->module_id[sizeof(arguments->module_id) - 1] = 0;
            break;

        case ARGP_KEY_END:

            // if (state->arg_num < 1)
            // {
            //     snprintf(arguments->module_id, sizeof(arguments->module_id), "%s", getenv("MODULE"));
            //     if (arguments->module_id[0] == 0)
            //     {
            //         fprintf(stderr, "Error: Too few arguments!\n");
            //         argp_usage (state);
            //     }
            // }

            if (arguments->server_queue[0] == 0) {
                fprintf(stderr, "Error: Option 'topic' is mandatory!\n");
                argp_usage (state);
            }

            break;

        default: 

            return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static char doc[] = "Random number genrator server.";
static char args_doc[] = "[SERVER ID]";
static struct argp_option options[] = { 
    { "logfile", 'l', "logfile path", 0, "Path to the logfile."},
    { "pidfile", 'p', "pidfile path", 0, "Path to the pidfile."},
    { "topic", 's', "queue name", 0, "Central communications queue."},
    { "no-daemon", 'n', 0, 0, "Don't run as a daemon."},
    { 0 } 
};

struct argp argp = { options, parse_opt, args_doc, doc};
