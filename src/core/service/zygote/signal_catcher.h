#ifndef SPARROW_SIGNAL_CATCHER_H
#define SPARROW_SIGNAL_CATCHER_H

#include <signal.h>
#include <pthread.h>

class SignalCatcher {
public:
  SignalCatcher();
  ~SignalCatcher();

private:
  static void* Run(void* arg);

  void HandleSigUsr1();

  int WaitForSignal(sigset_t signals);

  pthread_t pthread_;
};

#endif

