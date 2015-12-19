#ifndef SPARROW_RUNTIME_H
#define SPARROW_RUNTIME_H

#include <signal.h>
#include "signal_catcher.h"

class Runtime {
public:
  Runtime();
  ~Runtime();

  bool Init(); 
  void DidForkFromZygote();
  void StartSignalCatcher();
  static bool Create();
  static Runtime* Current();

private:
  void initPlatformSignalHandlers();
  void blockSignals();
  static void CrashSigHandler(int signal_number, siginfo_t* info, void* raw_context);

  static Runtime* instance_;

  bool is_zygote_;
  SignalCatcher* signal_catcher_;
};
#endif
