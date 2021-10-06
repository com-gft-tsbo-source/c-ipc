#ifdef __CONFIG_SERVER_H
#else
#define __CONFIG_SERVER_H

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <string.h>
#include <linux/limits.h>

static char doc[] = "Your program description.";
static char args_doc[] = "[CLIENT ID]";

static struct argp_option options[] = { 
    { 0 } 
};

struct arguments {
    char logfile[PATH_MAX + 1];
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {

        case ARGP_KEY_ARG: 

            if (state->arg_num >= 1)
            {
                argp_usage (state);
            }

            strncpy(arguments->logfile, arg, sizeof(arguments->logfile));
            arguments->logfile[sizeof(arguments->logfile) - 1] = 0;
            break;

        case ARGP_KEY_END:

            if (state->arg_num < 1)
            {
                fprintf(stderr, "Error: Too few arguments!\n");
                argp_usage (state);
            }

            break;

        default: 

            return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc};

#endif