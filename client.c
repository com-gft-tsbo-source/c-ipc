#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "ipc_common.h"
#include "config.h"
#include "config-client.h"
#include "logging.h"

int main(int argc, char **argv)
{
    struct arguments arguments;
    memset(&arguments, sizeof(arguments), 0);
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    struct logsetup ls;
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

    strncpy(module_id, arguments.module_id, sizeof(module_id));
    snprintf(module_name, sizeof(module_name), "%s-%08d", module_id, getpid());
    strncpy(server_queue, arguments.server_queue, sizeof(server_queue));
    snprintf(reply_queue, sizeof(reply_queue), "/%s-reply-%d", module_id, getpid());
    log_init(&ls, module_name, arguments.logfile, fileno(stderr));
    
    attr.mq_flags = 0;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = sizeof(msg_reply);
    attr.mq_curmsgs = 0;
    log_print(&ls, "This is module '%s'.\n", module_id);
    log_print(&ls, "Listening on queue '%s'.\n", reply_queue);
    log_print(&ls, "Requesting on queue '%s'.\n", server_queue);

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
    log_print(&ls, "requesting value.\n");

    if (mq_send(qd_server, (char*) &msg_request, sizeof(msg_request), 0) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_send!");
        perror(logbuffer);
        exit_status = 2;
        goto CLEANUP;
    }

    log_print(&ls, "waiting for reply.\n");

    if (mq_receive(qd_client,(char*) &msg_reply, sizeof(msg_reply), NULL) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "mq_receive!");
        perror(logbuffer);
        exit_status = 3;
        goto CLEANUP;
    }

    log_print(&ls, "received '%d' from '%.*s'\n", msg_reply.random_number, sizeof(msg_reply.module_name), msg_reply.module_name);

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

    log_print(&ls, "stopping.\n");

    exit(exit_status);
}