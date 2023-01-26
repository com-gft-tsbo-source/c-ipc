#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <bsd/unistd.h>
#include "ipc_common.h"
#include "config.h"
#include "config-server.h"
#include "daemon.h"
#include "logging.h"
#include "lockfile.h"

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

int main(int argc, char **argv, char **envp)
{
    extern struct argp argp;

    // -----------------------------------------------------------------------
    // Local variables

    struct arguments arguments;
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    long int counter = 0;
    char module_name[MAX_MODULE_NAME];
    char server_queue[MAX_QUEUE_NAME];
    char logfile_name[sizeof(arguments.logfile)];

    // -----------------------------------------------------------------------
    // parse CLI arguments

    memset(&arguments, 0, sizeof(arguments));
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    setproctitle_init(argc, argv, envp);

    // -----------------------------------------------------------------------
    // process CLI arguments

    if (arguments.module_id[0] == 0) {
        const char *ev = getenv("MODULE");
        if (ev != NULL)
            snprintf(arguments.module_id, sizeof(arguments.module_id), "%s", ev);
    }

    if (arguments.module_id[0] == '@') {
        const char *ev;
        arguments.module_id[0] = 0;

        if (arguments.module_id[1] == 0) {
            ev = getenv("HOSTNAME");
            if (ev != NULL)
                snprintf(arguments.module_id, sizeof(arguments.module_id), "%s", ev);
        } else {
            ev = getenv(arguments.module_id + 1);
            if (ev != NULL)
                snprintf(arguments.module_id, sizeof(arguments.module_id), "%s", ev);
        }
    }

    if (arguments.module_id[0] == 0) 
        if (arguments.server_queue[0] != 0) 
            snprintf(arguments.module_id, sizeof(arguments.module_id), "%s", arguments.server_queue);

    if (arguments.module_id[0] == 0)
    {
        fprintf(stderr, "Error: Too few arguments!\n");
        fprintf(stderr, "Either:\n");
        fprintf(stderr, "  - Pass the server name as the last cli argument.\n");
        fprintf(stderr, "  - Set the module environment variable.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(module_id, arguments.module_id, sizeof(module_id));

    if (arguments.server_queue[0] == 0) {
        snprintf(server_queue, sizeof(server_queue), "/%s-query", module_id);
    } else if (arguments.server_queue[0] != '/') {
        snprintf(server_queue, sizeof(server_queue), "/%s", arguments.server_queue);
    } else {
        strncpy(server_queue, arguments.server_queue, sizeof(server_queue));
    }

    snprintf(module_name, sizeof(module_name), "%s@%s", server_queue + 1, module_id);
    LOG_INIT( module_name, NULL, fileno(stdout));
    setproctitle("%s@%s", server_queue+1, module_id);

    if (arguments.logfile[0] != 0) {
        strncpy(logfile_name, arguments.logfile, sizeof(logfile_name));

        if (logfile_name[0] != '-' && logfile_name[1] != '\0' && logfile_name[0] != '/') {
            LOG_PRINT("Error: Logfile path must be absolute or '-'!\n");
            exit(EXIT_FAILURE);
        }

    }
    else {
        snprintf(logfile_name, sizeof(logfile_name), "/var/log/server/%s.log", module_name);
    }

    // -----------------------------------------------------------------------
    // Some initialization

    attr.mq_flags = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(msg_request);
    attr.mq_curmsgs = 0;

    LOG_CLOSE();
    if (logfile_name[0] == 0) LOG_INIT( module_name, NULL, fileno(stdout));
    else                      LOG_INIT( module_name, logfile_name, -1);

    LOG("This is module '%s'.\n", module_id);
    LOG("Listening on queue '%s'.\n", server_queue);
    if (logfile_name[0] != 0) LOG("Logfile is '%s'.\n", logfile_name);

    // -----------------------------------------------------------------------
    // Become a daemon if desired

    handle_signal(SIGHUP, SIG_IGN);
    handle_signal(SIGTERM, do_sigterm);
    handle_signal(SIGINT, do_sigint);

    if (! arguments.no_daemon) {
        LOG_CLOSE();

        to_daemon();

        if (logfile_name[0] == 0) LOG_INIT( module_name, NULL, -1);
        else                      LOG_INIT( module_name, logfile_name, -1);
    }

    // -----------------------------------------------------------------------
    // create PIDfile if desired

    if (arguments.pidfile[0] != 0) {
        create_pidfile(arguments.pidfile);
    }

    // -----------------------------------------------------------------------
    // Open listening queue

    qd_server = mq_open(server_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    
    if (qd_server == -1) {
        LOG_ERRNO("mq_open failed!.\n");
        exit(EXIT_FAILURE);
    }

    // -----------------------------------------------------------------------
    // Process events

    while (1) {

        // -------------------------------------------------------------------
        // Wait for first event

        memset(&msg_request, sizeof(msg_request), 0);
        LOG("waiting for request.\n");

        if (mq_receive(qd_server, (char*) &msg_request, sizeof(msg_request), NULL) == -1) {
            LOG_ERRNO("mq_receive failed!.\n");
            exit(EXIT_FAILURE);
        }

        LOG("request received from '%.*s', replying to '%-.*s'.\n", sizeof(msg_request.module_name), msg_request.module_name, sizeof(msg_request.reply_queue), msg_request.reply_queue);

        // -------------------------------------------------------------------
        // Reply
        // Open client reply queue

        if ((qd_client = mq_open(msg_request.reply_queue, O_WRONLY)) == 1) {
            LOG_ERRNO("mq_open failed!.\n");
            continue;
        }

        // Send the reply
        memset(&msg_reply, sizeof(msg_reply), 0);
        snprintf(msg_reply.module_name, sizeof(msg_reply.module_name), module_name );
        msg_reply.random_number = ++counter;

        if (mq_send(qd_client, (char*) &msg_reply, sizeof(msg_reply), 0) == -1) {
            LOG_ERRNO("mq_send failed!");
            continue;
        }

        LOG("sent reply '%d' to '%.*s'.\n", msg_reply.random_number, sizeof(msg_request.module_name), msg_request.module_name);

        // Close client queue
        if (mq_close(qd_client) == -1) {
            LOG_ERRNO("mq_close failed!.\n");
            exit(EXIT_FAILURE);
        }

    }
    LOG_CLOSE();
}
