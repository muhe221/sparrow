#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#include "log.h"
#include "util.h"

int create_socket(const char* name, int type) {
  // sockaddr_un can prevent hacker from attack.
  struct sockaddr_un addr;
  int fd, ret;

  // PF_UNIX is for IPC
  fd = socket(PF_UNIX, type, 0);
  if (fd < 0) {
    PLOG(FATAL) << "create socket fail.";
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", name);

  ret = unlink(addr.sun_path);
  ret = bind(fd, (struct sockaddr *) &addr, sizeof (addr));
  if (ret) {
    PLOG(FATAL) << "fail to bind.";
  }

  return fd;
}
