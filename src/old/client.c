/*
 * client.c: Client program
 *           to demonstrate interprocess communication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "ipc_common.h"

int main (int argc, char **argv)
{
    char logbuffer[512];
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    char client_name[MAX_CLIENT_NAME];
    char reply_queue[MAX_QUEUE_NAME];
    char server_queue[MAX_QUEUE_NAME];
    int exit_status = 0;

    if (argc > 1 ){
        snprintf(server_queue, sizeof(server_queue), "/%s-server", argv[1]);
        snprintf(client_name, sizeof(client_name), "%s-client-%08d", argv[1], getpid());
    } else {
        snprintf(server_queue, sizeof(server_queue), "/server");
        snprintf(client_name, sizeof(client_name), "client-%08d", getpid ());
    }

    sprintf (reply_queue, "/%s-reply", client_name);
    attr.mq_flags = 0;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = sizeof(msg_reply);
    attr.mq_curmsgs = 0;
    printf ("[%s]: starting, queue is '%s'.\n", client_name, reply_queue);

    if ((qd_client = mq_open(reply_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_open for reply queue failed!", client_name);
        perror(logbuffer);
        exit (1);
    }

    if ((qd_server = mq_open(server_queue, O_WRONLY)) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_open for server queue failed!", client_name);
        perror(logbuffer);
        exit_status = 1;
        goto CLEANUP;
    }

    memset(&msg_request, sizeof(msg_request), 0);
    snprintf(msg_request.client_name, sizeof(msg_request.client_name), client_name );
    snprintf(msg_request.reply_queue, sizeof(msg_request.reply_queue), reply_queue );
    printf ("[%s]: requesting value.\n", client_name);

    if (mq_send(qd_server, (char*) &msg_request, sizeof(msg_request), 0) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_send!", client_name);
        perror(logbuffer);
        exit_status = 2;
        goto CLEANUP;
    }

    printf ("[%s]: waiting for reply.\n", client_name);

    if (mq_receive(qd_client,(char*) &msg_reply, sizeof(msg_reply), NULL) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_receive!", client_name);
        perror(logbuffer);
        exit_status = 3;
        goto CLEANUP;
    }

    printf ("[%s]: received '%d' from '%-*s'\n", client_name, msg_reply.random_number, sizeof(msg_reply.server_name), msg_reply.server_name);

  CLEANUP:
 
    if (mq_close(qd_client) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_close!", client_name);
        perror(logbuffer);
        exit_status = 4;
        goto CLEANUP;
    }

    if (mq_unlink(reply_queue) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s]: mq_unlink!", client_name);
        perror(logbuffer);
        exit_status = 5;
        goto CLEANUP;
    }

    printf ("[%s]: stopping.\n", client_name);

    exit(exit_status);
}