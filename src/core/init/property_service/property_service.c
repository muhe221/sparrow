#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "system_properties.h"
#include "property_service.h"
#include "log.h"
#include "../base.h"
#include "util.h"


extern int _property_set_fd;

static int property_area_inited = 0;

static workspace pa_workspace;

//This just mmap for property service memory share
int system_property_init() {
  if (property_area_inited) {
    return -1;
  }

  if (__system_property_area_init() == -1) {
    LOG(FATAL) << "fail to init system property area.";
    return -1;
  }

  if (init_workspace() == -1) {
    return -1;
  }

  property_area_inited = 1;
  return 0;
}


int init_workspace()
{
  pa_workspace.size = 0;
  pa_workspace.fd = open(PROP_FILENAME, O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
  if (pa_workspace.fd == -1) {
    PLOG(FATAL) << "Failed to open";
    return -1;
  }

  return 0;
}

void start_property_service(int epoll_fd) {
  _property_set_fd = create_socket(PROP_SERVICE_NAME, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK);

  if (_property_set_fd == -1) {
    PLOG(FATAL) << "start_property_service socket creation failed";
  }

  listen(_property_set_fd, 8);

  register_epoll_handler(epoll_fd, _property_set_fd, handle_property_set_fd);
}

// property service set the property required by client.
// as to getting property, client can get it by shared memory.
void handle_property_set_fd() {
  prop_msg msg;

  int s;
  int r;

  struct ucred cr;
  struct sockaddr_un addr; //  sys/un.h
  socklen_t addr_size = sizeof(addr); //???
  socklen_t cr_size = sizeof(cr);

  if ((s = accept(_property_set_fd, (struct sockaddr*) &addr, &addr_size)) < 0 ) {
    PLOG(ERROR) << "fail to accept.";
    return;
  }

  // Check socket options here
  if (getsockopt(s, SOL_SOCKET, SO_PEERCRED, &cr, &cr_size) < 0) {
    close(s);
    PLOG(ERROR) << "Unable to recieve socket options";
    return;
  }

  r = recv(s, &msg, sizeof(msg), 0);
  close(s);

  if (r != sizeof(prop_msg)) {
    LOG(ERROR) <<"sys_prop: mis-match msg size recieved";
    return;
  }

  switch(msg.cmd) {
    case PROP_MSG_SETPROP:
      LOG(DEBUG) << "Porperty service get the msg as follow:";
      LOG(DEBUG) << "msg.name:" << msg.name;
      LOG(DEBUG) << "msg.value:" << msg.value;
      /*
      if (property_set(msg.name, msg.value) == -1) {
        LOG(ERROR) << "fail to set property name:" << msg.name << ", value:" << msg.value;
      } else {
        LOG(INFO) << "success to set property name:" << msg.name << ", value:" << msg.value;
      }
*/
      break;
    default:
      LOG(ERROR) << "Unknown msg cmd:" << msg.cmd;
      break;  
  }
}

/*
int property_set(const char* name, const char* value) {
  LOG(DEBUG) << "Porperty service get the msg as follow:";
  LOG(DEBUG) << "name:" << name;
  LOG(DEBUG) << "value:" << value;

  if (!is_legal_property_name(name, value)) {
     return -1;
  }

  //TODO: set the value to shared memory  
  //   It involves concurrent programing.
  
  return 0;
}

*/
int is_legal_property_name(const char* name, const char* value) {
  if (strlen(name) > PROP_NAME_MAX - 1 || strlen(value) > PROP_VALUE_MAX-1) {
     LOG(ERROR) << "the length of property name or value is not right.";
     return -1;
  } 

  //TODO: other check

  return 0;
}





