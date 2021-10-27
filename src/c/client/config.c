#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc_common.h"
#include "config.h"
#include "config-client.h"
#include "logging.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {

        case 'l': 

            strncpy(arguments->logfile, arg, sizeof(arguments->logfile));
            arguments->logfile[sizeof(arguments->logfile) - 1] = 0;
            break;

        case 't': 

            strncpy(arguments->server_queue, arg, sizeof(arguments->server_queue));
            arguments->server_queue[sizeof(arguments->server_queue) - 1] = 0;
            break;

        case ARGP_KEY_ARG: 

            if (state->arg_num >= 1)
            {
                fprintf(stderr, "Error: Too many arguments!\n");
                argp_usage (state);
            }

            strncpy(arguments->module_id, arg, sizeof(arguments->module_id));
            arguments->module_id[sizeof(arguments->module_id) - 1] = 0;
            break;

        case ARGP_KEY_END:

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

static char doc[] = "Random number generator client.";
static char args_doc[] = "[CLIENT ID]";
static struct argp_option options[] = { 
    { "logfile", 'l', "logfile path", 0, "Path to the logfile."},
    { "topic", 't', "queue name", 0, "Central communications queue."},
    { 0 } 
};

struct argp argp = { options, parse_opt, args_doc, doc};
