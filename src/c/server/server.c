#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "ipc_common.h"
#include "config.h"
#include "config-server.h"
#include "logging.h"
#include "lockfile.h"

struct logsetup *ls = &global_ls;

typedef void (*sighandler_t)(int);
static sighandler_t handle_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction neu_sig, alt_sig;
   neu_sig.sa_handler = signalhandler;
   sigemptyset (&neu_sig.sa_mask);
   neu_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
      return SIG_ERR;
   return alt_sig.sa_handler;
}

static void do_sigterm (int sig_nr) {
    LOG("Terminating for SIGTERM.\n");
    exit(1);
}

static void do_sigint (int sig_nr) {
    LOG("Terminating for SIGINT.\n");
    exit(1);
}

static void to_daemon(){
    pid_t pid;

    pid = fork();

    if(pid < 0) exit(EXIT_FAILURE);

    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();

    if(pid < 0) exit(EXIT_FAILURE);

    if (pid > 0) exit(EXIT_SUCCESS);

    chdir ("/");
    umask (0);

    int fd0, fd1, fd2;
    int i;

    for (i = sysconf(_SC_OPEN_MAX); i > 0; i--)
        close (i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

}

int create_pidfile(const char* pidfile)
{
    int fd;
    char buf[16];
    size_t len=0;

    LOG("Using pidfile '%s'.\n", pidfile);
    fd = open(pidfile, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    if (fd < 0) {
        LOG_ERRNO("Can′t open pidfile '%s'.\n", pidfile);
        exit(1);
    }

    if (lockfile(fd) < 0) {
        LOG_ERRNO("Can′t lock pidfile '%s'.\n", pidfile);
        exit(1);
    }

    ftruncate(fd, 0);
    len = snprintf(buf, sizeof(buf), "%ld", (long)getpid());
    write(fd, buf, len);
    return(0);
}


int main(int argc, char **argv)
{
    // -----------------------------------------------------------------------
    // Local variables
    char logbuffer[512];
    mqd_t qd_server;
    mqd_t qd_client; 
    struct mq_attr attr;
    struct message_request msg_request;
    struct message_reply msg_reply;
    long int counter = 0;
    char module_name[MAX_MODULE_NAME];
    char server_queue[MAX_QUEUE_NAME];
    char logfile_name[sizeof(ls->logfile_name)];

    // -----------------------------------------------------------------------
    // parse CLI arguments

    struct arguments arguments;
    memset(&arguments, 0, sizeof(arguments));
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // -----------------------------------------------------------------------
    // process CLI arguments

    strncpy(module_id, arguments.module_id, sizeof(module_id));
    snprintf(module_name, sizeof(module_name), "%s-%08d", module_id, getpid());

    if (arguments.server_queue[0] == 0) {
        snprintf(arguments.server_queue, sizeof(arguments.server_queue), "/%s-query", module_id);
    }

    strncpy(server_queue, arguments.server_queue, sizeof(server_queue));

    if (arguments.logfile[0] != 0) {
        strncpy(logfile_name, arguments.logfile, sizeof(logfile_name));
    }
    else {
        snprintf(logfile_name, sizeof(logfile_name), "/var/log/%s.log", module_name);
    }

    // -----------------------------------------------------------------------
    // Some initialization

    attr.mq_flags = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(msg_request);
    attr.mq_curmsgs = 0;

    if (logfile_name[0] == 0) LOG_INIT( module_name, NULL, fileno(stderr));
    else                      LOG_INIT( module_name, logfile_name, 0);

    LOG("This is module '%s'.\n", module_id);
    LOG("Listening on queue '%s'.\n", server_queue);
    if (logfile_name[0] != 0) LOG("Logfile is '%s'.\n", logfile_name);

    // -----------------------------------------------------------------------
    // Become a daemon if desired

    handle_signal(SIGHUP, SIG_IGN);
    handle_signal(SIGTERM, do_sigterm);
    handle_signal(SIGINT, do_sigint);

    if (! arguments.no_daemon) {
        to_daemon();
    }

    // -----------------------------------------------------------------------
    // create PIDfile if desired

    if (arguments.pidfile[0] != 0) {
        create_pidfile(arguments.pidfile);
    }

    // -----------------------------------------------------------------------
    // Open listening queue

    if ((qd_server = mq_open(server_queue, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        LOG_ERRNO("mq_open failed!.\n");
        exit(1);
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
            exit(1);
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
            exit(1);
        }

    }
    LOG_CLOSE();
}