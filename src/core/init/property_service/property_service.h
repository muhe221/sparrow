#ifndef SPARROW_PROPERTY_SERVICE_H
#define SPARROW_PROPERTY_SERVICE_H
#include <stdio.h>


//#define PROP_SERVICE_NAME "property_service"

struct workspace {
    size_t size;
    int fd;
};

void start_property_service(int epoll_fd);
int system_property_init();

void handle_property_set_fd();
int init_workspace();
int is_legal_property_name(const char* name, const char* value);

int property_set(const char* name, const char* value);
#endif
