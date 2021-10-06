#ifdef __IPC_COMMON_H
#else
#define __IPC_COMMON_H

#define QUEUE_PERMISSIONS 0660
#define MAX_QUEUE_SIGNATURE 64

#define MAX_ID 32
#define MAX_MODULE_NAME MAX_ID + 1 + 64

struct message_request {
    char module_name[MAX_MODULE_NAME];
    char reply_queue[1 + MAX_MODULE_NAME + 1 + MAX_QUEUE_SIGNATURE];
};

struct message_reply {
    char module_name[MAX_MODULE_NAME];
    int random_number;
};

#define MAX_QUEUE_NAME sizeof(((struct message_request *)0)->reply_queue)

#endif