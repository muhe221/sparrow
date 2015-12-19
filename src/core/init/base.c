#include <sys/epoll.h>
#include <sys/socket.h>

#include "base.h"
#include "log.h"

void register_epoll_handler(int epoll_fd, int fd, void (*fn)()) {
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = reinterpret_cast<void*>(fn); //1111111
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      PLOG(FATAL) << "epoll_ctl failed";
    }
}

