#ifndef SPARROW_BASE_H
#define SPARROW_BASE_H

void register_epoll_handler(int epoll_fd, int fd, void (*fn)());

#endif
