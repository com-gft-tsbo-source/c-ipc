#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <bsd/unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "ipc_common.h"
#include "daemon.h"
#include "config.h"
#include "config-client.h"
#include "logging.h"

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

    char logbuffer[512];
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    char module_name[MAX_MODULE_NAME];
    char server_queue[MAX_QUEUE_NAME];
    char reply_queue[MAX_QUEUE_NAME];
    int exit_status = 0;

    struct arguments arguments;
    memset(&arguments, sizeof(arguments), 0);
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    setproctitle_init(argc, argv, envp);

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
        if (arguments.server_queue != 0)
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

    if (arguments.server_queue[0] != '/') {
        snprintf(server_queue, sizeof(server_queue), "/%s", arguments.server_queue);
    } else {
        strncpy(server_queue, arguments.server_queue, sizeof(server_queue));
    }

    snprintf(module_name, sizeof(module_name), "%s#%s", module_id, server_queue + 1);

    snprintf(reply_queue, sizeof(reply_queue), "/%s-reply-%d", module_id, getpid());
    LOG_INIT( module_name, arguments.logfile, fileno(stdout));
    setproctitle("%s@%s", server_queue+1, module_id);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = sizeof(msg_reply);
    attr.mq_curmsgs = 0;
    LOG_PRINT("This is module '%s'.\n", module_id);
    LOG_PRINT("Listening on queue '%s'.\n", reply_queue);
    LOG_PRINT("Requesting on queue '%s'.\n", server_queue);

    handle_signal(SIGHUP, SIG_IGN);
    handle_signal(SIGTERM, do_sigterm);
    handle_signal(SIGINT, do_sigint);

    if ((qd_client = mq_open(reply_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_open for reply queue failed!");
        perror(logbuffer);
        exit(1);
    }

    if ((qd_server = mq_open(server_queue, O_WRONLY)) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_open for server queue failed!");
        perror(logbuffer);
        exit_status = 1;
        goto CLEANUP;
    }

    memset(&msg_request, sizeof(msg_request), 0);
    snprintf(msg_request.module_name, sizeof(msg_request.module_name), module_name );
    snprintf(msg_request.reply_queue, sizeof(msg_request.reply_queue), reply_queue );
    LOG_PRINT("requesting value.\n");

    if (mq_send(qd_server, (char*) &msg_request, sizeof(msg_request), 0) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_send!");
        perror(logbuffer);
        exit_status = 2;
        goto CLEANUP;
    }

    LOG_PRINT("waiting for reply.\n");

    if (mq_receive(qd_client,(char*) &msg_reply, sizeof(msg_reply), NULL) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_receive!");
        perror(logbuffer);
        exit_status = 3;
        goto CLEANUP;
    }

    LOG_PRINT("received '%d' from '%.*s'.\n", msg_reply.random_number, sizeof(msg_reply.module_name), msg_reply.module_name);

  CLEANUP:
 
    if (mq_close(qd_client) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_close!");
        perror(logbuffer);
        exit_status = 4;
        goto CLEANUP;
    }

    if (mq_unlink(reply_queue) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_unlink!");
        perror(logbuffer);
        exit_status = 5;
        goto CLEANUP;
    }

    LOG_PRINT("stopping.\n");
    LOG_CLOSE();
    exit(exit_status);
}
