#ifndef SPARROW_FAULT_HANDLER_H
#define SPARROW_FAULT_HANDLER_H

#include <signal.h>
#include <vector>

static void art_fault_handler(int sig, siginfo_t* info, void* context);

class FaultHandler;

class FaultManager {
 public:
  FaultManager();
  ~FaultManager();

  void Init();
  void Shutdown();
  void EnsureArtActionInFrontOfSignalChain();

  void HandleFault(int sig, siginfo_t* info, void* context);
  void AddHandler(FaultHandler* handler, bool generated_code);
  void RemoveHandler(FaultHandler* handler);

 private:
  std::vector<FaultHandler*> generated_code_handlers_;
  std::vector<FaultHandler*> other_handlers_;
  struct sigaction oldaction_;
  bool initialized_;
};


class FaultHandler {
 public:
  explicit FaultHandler(FaultManager* manager);
  virtual ~FaultHandler() {}
  FaultManager* GetFaultManager() {
    return manager_;
  }

  virtual bool Action(int sig, siginfo_t* siginfo, void* context) = 0;

 protected:
  FaultManager* const manager_;
};

class NullPointerHandler : public FaultHandler {
 public:
  explicit NullPointerHandler(FaultManager* manager);

  bool Action(int sig, siginfo_t* siginfo, void* context);
};


class StackOverflowHandler : public FaultHandler {
 public:
  explicit StackOverflowHandler(FaultManager* manager);

  bool Action(int sig, siginfo_t* siginfo, void* context);
};
#endif
