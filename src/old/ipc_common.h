#ifdef __IPC_COMMON_H
#else
#define __IPC_COMMON_H

#define SERVER_QUEUE_NAME   "/sp-example-server"

#define QUEUE_PERMISSIONS 0660
#define MAX_CLIENT_NAME 64
#define MAX_QUEUE_SIGNATURE 64

struct message_request {
    char client_name[MAX_CLIENT_NAME];
    char reply_queue[1 + MAX_CLIENT_NAME + 1 + MAX_QUEUE_SIGNATURE];
};

struct message_reply {
    char server_name[MAX_CLIENT_NAME];
    int random_number;
};

#define MAX_QUEUE_NAME sizeof(((struct message_request *)0)->reply_queue)

#endif