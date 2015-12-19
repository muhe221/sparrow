#include <stdlib.h>
#include <sys/socket.h>  
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "zygote.h"
#include "runtime.h"
#include "log.h"
#include "config.h"
#include "start.h"
#include "properties.h"

void forkSystemServer() {
  LOG(INFO) << "fork system server.";

  pid_t pid = fork();
  if (pid < 0) {
    PLOG(FATAL) << "Fail to fork SystemServer.";
  } else if (pid == 0) {
    LOG(INFO) << "startSystemServer()...";
    startSystemServer();
    PLOG(FATAL) << "System server exit.";
  }

  LOG(INFO) << "zygote fork SystemServer pid: " << pid;

  // why try to check the status of SystemServer?
  // SystemServer may die ?
  // WNOHANG: non-block
  int status;
  if (waitpid(pid, &status, WNOHANG) == pid) { // sys/wait.h
    // If SystemServer died, kill myself.
    PLOG(FATAL) << "system server died.";
    kill(getpid(), SIGKILL);
  }
}

void startSystemServer() {
  int time = 0;
  while(1) {
    if ((++time) % 4 == 0) {
      LOG(DEBUG) << "zygote begin to set prop";
      property_set("caoming", "12306");
    }

    sleep(4);
    LOG(DEBUG) << "SystemServer is running.";
  }
}

void registerZygoteSocket() {
  int socket_fd, connect_fd;
  struct sockaddr_in servaddr;
  //char buff[4096];

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    PLOG(FATAL) << "Create socket error.";
  }

  memset(&servaddr, 0, sizeof(servaddr));  // string.h
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htonl(DEFAULT_PORT);

  if (bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    PLOG(FATAL) << "Bind socket error.";
  }

  if (listen(socket_fd, 10) == -1) {
    PLOG(FATAL) << "Listen socket error.";
  }

  while (1) {
    LOG(INFO) << "======Wait for client's request======";

    if ((connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1) {
      LOG(ERROR) << "Accept socket error.";
      continue;
    }

    pid_t pid = fork();
    if (pid < 0) {
       PLOG(ERROR) << "Fail to fork()";
    } else if (pid == 0) {
       

    }

    close(connect_fd);
  }

  close(socket_fd);
}

void PrepareForSpawn() {
  registerZygoteSocket();
}

int main(int argc, char *argv[])
{
 // for property service memory share.
  __start_main();

  LOG(INFO) << "Create Runtime.";
  if (!Runtime::Create()) {
    LOG(FATAL) << "Fail to create Runtime";
  }

  forkSystemServer();
  LOG(INFO) << "Try to prepare for spawn process";

  PrepareForSpawn();

  LOG(FATAL) << "exit(1)";
  exit(1);
}

