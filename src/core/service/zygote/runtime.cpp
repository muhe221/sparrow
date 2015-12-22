#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "runtime.h"
#include "macro.h"
#include "log.h"

Runtime* Runtime::instance_ = NULL;

Runtime::Runtime() {

}

Runtime::~Runtime() {

}

bool Runtime::Create() {
  if (instance_ != NULL) {
     return false;
  }

  instance_ = new Runtime;
  if (!instance_->Init()) {
    delete instance_;
    instance_ = NULL;
    return false;
  }

  return true;
}

bool Runtime::Init() {
  blockSignals();
  initPlatformSignalHandlers();

  return true;
}

Runtime* Runtime::Current() {
  return instance_;
}

void Runtime::initPlatformSignalHandlers()
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));  // string.h
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = CrashSigHandler;
    action.sa_flags |= SA_SIGINFO;
    action.sa_flags |= SA_ONSTACK; // may be not used;

    int rc = 0;
    rc += sigaction(SIGABRT, &action, NULL);
    rc += sigaction(SIGSEGV, &action, NULL);
    CHECK_EQ(rc, 0);
    LOG(INFO) << "Init signal handlers finish";
}

void Runtime::blockSignals()
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_BLOCK, &set, NULL);

  LOG(INFO) << "block signals finish";
}

void Runtime::DidForkFromZygote() {
  is_zygote_ = false;
  StartSignalCatcher(); 
}

void Runtime::StartSignalCatcher() {
  if (!is_zygote_) {
//    signal_catcher_ = new SignalCatcher;
  }
}

void Runtime::CrashSigHandler(int signal_number, siginfo_t* info, void* raw_context) {
    LOG(ERROR) << "===========crashSigHandler signal:" << signal_number << "==============";
    LOG(DEBUG) << "thread tid:" << pthread_self();
    sleep(5); // unistd.h
}

