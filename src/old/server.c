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
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    long int counter = 0;
    char server_name[MAX_CLIENT_NAME];
    char server_queue[MAX_QUEUE_NAME];

    if (argc > 1 ){
        snprintf(server_queue, sizeof(server_queue), "/%s-server", argv[1]);
        sprintf (server_name, "%s-server-%08d", argv[1], getpid());
    } else {
        snprintf(server_queue, sizeof(server_queue), "/server");
        sprintf (server_name, "server-%08d", getpid());
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(msg_request);
    attr.mq_curmsgs = 0;
    printf ("[%s]: starting, queue is '%s'.\n", server_name, server_queue);

    if ((qd_server = mq_open(server_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        LOG_ERRNO("mq_open failed on server queue!.\n");
        exit (1);
    }

    while (1) {

        memset(&msg_request, sizeof(msg_request), 0);
        printf ("[%s]: waiting for request.\n", server_name);

        if (mq_receive(qd_server, (char*) &msg_request, sizeof(msg_request), NULL) == -1) {
            LOG_ERRNO("mq_received failed on server queue!.\n");
            exit (1);
        }

        printf ("[%s]: request received from '%-.*s', replying to '%-.*s'.\n", server_name, sizeof(msg_request.client_name), msg_request.client_name, sizeof(msg_request.reply_queue), msg_request.reply_queue);

        if ((qd_client = mq_open(msg_request.reply_queue, O_WRONLY)) == 1) {
            LOG_ERRNO("mq_open failed on reply queue!.\n");
            continue;
        }

        memset(&msg_reply, sizeof(msg_reply), 0);
        snprintf(msg_reply.server_name, sizeof(msg_reply.server_name), server_name );
        msg_reply.random_number = ++counter;

        if (mq_send(qd_client, (char*) &msg_reply, sizeof(msg_reply), 0) == -1) {
            LOG_ERRNO("mq_send failed on reply queue!.\n");
            continue;
        }

        printf ("[%s]: sent reply '%d' to '%-.*s'.\n", server_name, msg_reply.random_number, sizeof(msg_request.client_name), msg_request.client_name);

        if (mq_close(qd_client) == -1) {
            LOG_ERRNO("mq_close failed on reply queue!.\n");
            exit (1);
        }

    }
}