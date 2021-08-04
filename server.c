#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ipc_common.h"
#include "config.h"
#include "config-server.h"
#include "logging.h"

typedef void (*sighandler_t)(int);
static sighandler_t 
handle_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction neu_sig, alt_sig;
   neu_sig.sa_handler = signalhandler;
   sigemptyset (&neu_sig.sa_mask);
   neu_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
      return SIG_ERR;
   return alt_sig.sa_handler;
}

static void to_daemon(){
    int i;
    pid_t pid;

    pid = fork();

    if(pid < 0) exit(EXIT_FAILURE);

    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    handle_signal(SIGHUP, SIG_IGN);
    pid = fork();

    if(pid < 0) exit(EXIT_FAILURE);

    if (pid > 0) exit(EXIT_SUCCESS);

    chdir ("/");
    umask (0);

    for (i = sysconf(_SC_OPEN_MAX); i > 0; i--)
        close (i);
}

int main(int argc, char **argv)
{
    struct arguments arguments;
    memset(&arguments, 0, sizeof(arguments));
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    struct logsetup ls;
    char logbuffer[512];
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    long int counter = 0;
    char module_name[MAX_MODULE_NAME];
    char server_queue[MAX_QUEUE_NAME];

    strncpy(module_id, arguments.module_id, sizeof(module_id));
    snprintf(module_name, sizeof(module_name), "%s-%08d", module_id, getpid());

    if (arguments.server_queue[0] == 0) {
        snprintf(arguments.server_queue, sizeof(arguments.server_queue), "/%s-query", module_id);
    }

    strncpy(server_queue, arguments.server_queue, sizeof(server_queue));
    attr.mq_flags = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(msg_request);
    attr.mq_curmsgs = 0;
    log_init(&ls, module_name, arguments.logfile, fileno(stderr));
    log_print(&ls, "This is module '%s'.\n", module_id);
    log_print(&ls, "Listening on queue '%s'.\n", server_queue);

    if (! arguments.no_daemon) {
        to_daemon();
    }

    if ((qd_server = mq_open(server_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        snprintf(logbuffer, sizeof(logbuffer), "[%s] mq_open failed!");
        perror(logbuffer);
        exit(1);
    }

    while (1) {

        memset(&msg_request, sizeof(msg_request), 0);
       log_print(&ls, "waiting for request.\n");

        if (mq_receive(qd_server, (char*) &msg_request, sizeof(msg_request), NULL) == -1) {
            snprintf(logbuffer, sizeof(logbuffer), "[%s] mq_received failed!");
            perror(logbuffer);
            exit(1);
        }

       log_print(&ls, "request received from '%.*s', replying to '%-.*s'.\n", sizeof(msg_request.module_name), msg_request.module_name, sizeof(msg_request.reply_queue), msg_request.reply_queue);

        if ((qd_client = mq_open(msg_request.reply_queue, O_WRONLY)) == 1) {
            snprintf(logbuffer, sizeof(logbuffer), "[%s] mq_open failed!");
            perror(logbuffer);
            continue;
        }

        memset(&msg_reply, sizeof(msg_reply), 0);
        snprintf(msg_reply.module_name, sizeof(msg_reply.module_name), module_name );
        msg_reply.random_number = ++counter;

        if (mq_send(qd_client, (char*) &msg_reply, sizeof(msg_reply), 0) == -1) {
            snprintf(logbuffer, sizeof(logbuffer), "[%s] mq_send failed!");
            perror(logbuffer);
            continue;
        }

       log_print(&ls, "sent reply '%d' to '%.*s'.\n", msg_reply.random_number, sizeof(msg_request.module_name), msg_request.module_name);

        if (mq_close(qd_client) == -1) {
            snprintf(logbuffer, sizeof(logbuffer), "[%s] mq_close!");
            perror(logbuffer);
            exit(1);
        }

    }
}