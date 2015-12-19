#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stddef.h>
#include <sys/un.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include "system_properties.h"
#include "properties.h"

int property_set(const char *key, const char* value) {
  return __system_property_set(key, value);
}

