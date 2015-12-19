#include <signal.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include "macro.h"
#include "log.h"
#include "init.h"
#include "base.h"
#include "property_service/property_service.h"

// for property service
int _property_set_fd = -1;

// for SIG_CHLD
static int _signal_write_fd = -1;
static int _signal_read_fd = -1;


static bool _waiting_for_exec = false;

static int _epoll_fd = -1;
static const char *ENV[16];

// Android is 64
#define INIT_PARSER_MAXARGS 12


int main(int argc, char** argv)
{
  create_poll();

  //property_load_boot_defaults();
  if (system_property_init() == -1) {
    PLOG(FATAL) << "fail to init system property";
  } else {
    start_property_service(_epoll_fd);
  }

  init_parse_config_file("/init.rc");

  // TODO: Here should be modified according to andriod framework
  start_services();

  LOG(INFO) << "init process begin to while() { }";

  while (true) {
    if (!_waiting_for_exec) {
      // execute_one_command();
      // restart_process();
    }

    epoll_event ev;
    //int nr = TEMP_FAILURE_RETRY(epoll_wait(epoll_fd, &ev, 1, timeout));
    // timeout = 2000 millionseconds


    sleep(2);
    LOG(DEBUG) << "epoll_wait() ";

    int nr = TEMP_FAILURE_RETRY(epoll_wait(_epoll_fd, &ev, 1, 2000));
    LOG(DEBUG) << "epoll_wait() next while()";
    if (nr == -1) {
      LOG(ERROR) << "epoll_wait failed.";
    } else if (nr == 1) {
      ((void(*)()) ev.data.ptr)();
    } else {
      LOG(INFO) << "epoll_wait() nr:" << nr;
    }


    //TODO: deal with poll event

  }

  exit(0);
}

void create_poll() {
  // EPOLL_CLOEXEC: it will set flag close-on-exec(FD_CLOEXEC) to the epoll_fd
  // Other processes are abondaned to use epoll_fd
  _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
  if (_epoll_fd == -1) {
    LOG(FATAL) << "epoll_carete1 failed.";
  }

  LOG(DEBUG) << "epoll_create1() epoll_fd:" << _epoll_fd;
  signal_handler_init();
  //reap_any_outstanding_children();
  
  register_epoll_handler(_epoll_fd, _signal_read_fd, handle_signal);
}

void signal_handler_init() {
  int s[2];

  struct sigaction act; // not global is right? 111111
  memset(&act, 0, sizeof(act));
  // Only receive the finishing signal from sub process.
  act.sa_flags = SA_NOCLDSTOP;
  sigemptyset(&act.sa_mask);
  act.sa_handler = &sigchld_handler;
  sigaction(SIGCHLD, &act, NULL);

  //if (socketpair(AF_UNIX, SOCK_STREAM, 0 , s))
  if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0 , s)) {
    PLOG(FATAL) << "fail to create socketpair.";
  }

  
  _signal_write_fd = s[0];
  _signal_read_fd = s[1];
}

void sigchld_handler(int signal) {
  LOG(WARN) << "sigchld_handler()" << " signal:" << signal;

  //write(signal_fd, &signal, 1); 
  if (TEMP_FAILURE_RETRY(write(_signal_write_fd, "1", 1)) == -1) {
    PLOG(ERROR) << "fail to write signal to signal_write_fd.";
  }
}

void reap_any_outstanding_children() {
  while (wait_for_one_process()) {}
}

int wait_for_one_process() {
  //TODO: deal with SIGCHLD
  sleep(3);
  return 0;
}

void handle_signal() {
    LOG(WARN) << "init process handle signal"; 
#if 0
    // Clear outstanding requests ?
    char buf[32];
    // how to use it? or just for clear it?
    read(_signal_read_fd, buf, sizeof(buf));
#endif

    reap_any_outstanding_children();
}

void init_parse_config_file(const char *file) {
  //TODO: to parse file

}

// WorkAround: this method should be replaced.
// Here should start all services
void start_services() {

  pid_t pid = fork();
  if (pid < 0) {
    LOG(FATAL) << "failed to start service.";    
  } else if (pid == 0) {
    // system("gnome-terminal -e ./zygote");  
    //execvp("./zygote", (char* const*)NULL);
    const char *arg_ptrs[INIT_PARSER_MAXARGS + 1];
    arg_ptrs[0] = "123456";
    arg_ptrs[1] = '\0';
    ENV[0] = "123";
    ENV[1] = '\0';
    execve("./zygote", (char**) arg_ptrs, (char**) ENV);
    //execlp("./zygote", NULL, NULL);
    //char *argv[] = {"gnome-terminal", "-x", "bash", "-c", "/home/mutian/soft/compile/sparrow/out/zygote", (char*)0 };
    // execvp("gnome-terminal", argv);
    PLOG(FATAL) << "zygote process exit.";
  }

  LOG(INFO) << "init for process zygote pid: " << pid;
}
