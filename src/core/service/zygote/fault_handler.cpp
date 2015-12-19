#include <stdio.h>

#include "log.h"
#include "fault_handler.h"

FaultManager fault_manager;

FaultManager::FaultManager() : initialized_(false) {
  sigaction(SIGSEGV, NULL, &oldaction_);
}

void FaultManager::Init() {
  struct sigaction action; 
  action.sa_sigaction = art_fault_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO | SA_ONSTACK;

  int e = sigaction(SIGSEGV, &action, &oldaction_);
  if (!e) {
    PLOG(FATAL) << "Failed to claim SEGV.";
  }

//ClaimSignalChain(SIGSEGV, &oldaction_);
  initialized_ = true;
}

void FaultManager::Shutdown() {
  if (initialized_) {
   // UnclaimSignalChain(SIGSEGV);
    initialized_ = false;
  }
}


void FaultManager::HandleFault(int sig, siginfo_t* info, void* context) {
  LOG(ERROR) << "FaultManager::HandleFault() signal:" << sig;
/*
//  if (IsInGeneratedCode(info, context, true)) {
    for (const auto& handler : generated_code_handlers_) {
      if (handler->Action(sig, info, context)) {
        return;
      }
    }
//  }

  LOG(VERBOSE) << "try to deal with other_handers_";
  for (const auto& handler : other_handlers_) {
    if (handler->Action(sig, info, context)) {
      return;
    }
  }
*/

  LOG(VERBOSE) << "try to deal with signal other_handers_";
//  InvokeUserSignalHandler(sig, info, context);
}

FaultManager::~FaultManager() {

}

static void art_fault_handler(int sig, siginfo_t* info, void* context) {
  fault_manager.HandleFault(sig, info, context);
}
