#ifdef __DAEMON_H
#else
#define __DAEMON_H

typedef void (*sighandler_t)(int);
sighandler_t handle_signal (int sig_nr, sighandler_t signalhandler);
void to_daemon();
int create_pidfile(const char* pidfile);

#endif