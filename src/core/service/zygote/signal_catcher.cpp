#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h>

#include "macro.h"
#include "signal_catcher.h"
#include "log.h"

SignalCatcher::SignalCatcher() {
 //  CHECK_PTHREAD_CALL(pthread_create, (&pthread_, NULL, &Run, this), "signal catcher thread");
  // TODO: cond_.wait()
}

SignalCatcher::~SignalCatcher() {
//  CHECK_PTHREAD_CALL(pthread_kill, (pthread_, SIGQUIT), "signal catcher shutdown");
//  CHECK_PTHREAD_CALL(pthread_join, (pthread_, NULL), "signal catcher shutdown");
}

int SignalCatcher::WaitForSignal(sigset_t set) {
  int signal_number;
  int rc = TEMP_FAILURE_RETRY(sigwait(&set, &signal_number));
  if (rc != 0) {
    LOG(FATAL) << "sigwait failed";
  }

  return signal_number;  
}

void* SignalCatcher::Run(void* arg) {
  SignalCatcher* signal_catcher = reinterpret_cast<SignalCatcher*>(arg);

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGUSR1);

  while (true) {
    int signal_number = signal_catcher->WaitForSignal(set); 
    LOG(ERROR) << "catch signal:" << signal_number;
    if (false) {
      // TODO: when to return NULL
      return NULL;
    }
  }
}
