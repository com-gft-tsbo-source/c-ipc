#include <signal.h>
#include "daemon.h"

// ###########################################################################
// ###########################################################################
// Signal handling
// ###########################################################################
// ###########################################################################

sighandler_t handle_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction new_sig, old_sig;
   new_sig.sa_handler = signalhandler;
   sigemptyset (&new_sig.sa_mask);
   new_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &new_sig, &old_sig) < 0)
      return SIG_ERR;
   return old_sig.sa_handler;
}

