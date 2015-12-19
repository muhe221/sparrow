#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stddef.h>
#include <sys/un.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include "macro.h"
#include "log.h"
#include "system_properties.h"


int _prop_area_data_size_;
int _pa_size_;
prop_area * _system_property_area_ = NULL;


// sub process of init mmap prop aera. however, it just has the read only permission.
static int map_fd_ro(const int fd) {
    struct stat fd_stat;
    if (fstat(fd, &fd_stat) < 0) {
        return -1;
    }
/*
    if ((fd_stat.st_uid != 0)
            || (fd_stat.st_gid != 0)
            || ((fd_stat.st_mode & (S_IWGRP | S_IWOTH)) != 0)
            || (fd_stat.st_size < static_cast<off_t>(sizeof(prop_area))) ) {
        return -1;
    }
*/
    // In fact, it should be PAGE_SIZE
    // same as the process in init
    _pa_size_ = fd_stat.st_size;
    _prop_area_data_size_ = _pa_size_ - sizeof(prop_area);

    void* const map_result = mmap(NULL, _pa_size_, PROT_READ, MAP_SHARED, fd, 0);
    if (map_result == MAP_FAILED) {
      PLOG(FATAL) << "failed to mmap.";
      return -1;
    }

    prop_area* pa = reinterpret_cast<prop_area*>(map_result);
/*
    if ((pa->magic != PROP_AREA_MAGIC) || (pa->version != PROP_AREA_VERSION &&
                pa->version != PROP_AREA_VERSION_COMPAT)) {
        munmap(pa, pa_size);
        return -1;
    }

    if (pa->version == PROP_AREA_VERSION_COMPAT) {
        compat_mode = true;
    }
*/

   // sub process: to set __system_property_area__ to remaped mem address.
    _system_property_area_ = pa;
    return 0;
}



static int send_prop_msg(prop_msg *msg) {
  const int fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0) {
    PLOG(ERROR) << "fail to create socket.";
    return -1;
  }

  const size_t namelen = strlen(PROP_SERVICE_NAME);
  sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  //strlcpy(addr.sun_path, PROP_SERVICE_NAME, sizeof(addr.sun_path));
  strcpy(addr.sun_path, PROP_SERVICE_NAME);
  addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
  addr.sun_family = AF_LOCAL;
  socklen_t alen = namelen + offsetof(sockaddr_un, sun_path) + 1;
  if (TEMP_FAILURE_RETRY(connect(fd, reinterpret_cast<sockaddr*>(&addr), alen)) < 0) {
     close(fd);
     return -1;
  }

  int result = -1;
  const int num_bytes = TEMP_FAILURE_RETRY(send(fd, msg, sizeof(prop_msg), 0));

  if (num_bytes == sizeof(prop_msg)) {
    pollfd pollfds[1];
    pollfds[0].fd = fd;
    pollfds[0].events = 0;
    const int poll_result = TEMP_FAILURE_RETRY(poll(pollfds, 1, 250/* ms */));
    if (poll_result == 1 && (pollfds[0].revents & POLLHUP) != 0) {
      result = 0;
    } else {
      // ignore it.
      PLOG(WARN) << "poll timeout.";
      result = 0;
    }
  }

  return result;
}


// every process should call this method to share memory for getting property
int __system_property_area_init() {
  const int fd = open(PROP_FILENAME,
      O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_EXCL, 0444);

  if (fd < 0) {
    LOG(FATAL) << "fail to open file";
    return -1;
  }

  // I think here is duplicated to set FD_CLOEXEC.
  // Can it be removed ?
  const int ret = fcntl(fd, F_SETFD, FD_CLOEXEC);
  if (ret < 0) {
    close(fd);
    return -1;
  }

  if (ftruncate(fd, PAGE_SIZE) < 0) {
    close(fd);
    return -1;
  }


  _prop_area_data_size_ = PAGE_SIZE - sizeof(prop_area);
  void *const memory_area = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (memory_area == MAP_FAILED) {
    close(fd);
    return -1;
  }

  // create a prop_area object
  prop_area *pa = new(memory_area) prop_area(1, 1);
  _system_property_area_ = pa;

  close(fd);
  return 0;
}

int __map_prop_area()
{
    const int fd = open(PROP_FILENAME, O_CLOEXEC | O_NOFOLLOW | O_RDONLY);
    if (fd == -1 && errno == ENOENT) {
      PLOG(FATAL) << "failed to open property file.";
      return -1;
    }

    const int map_result = map_fd_ro(fd);
    close(fd);

    return map_result;
}

int __system_property_set(const char *key, const char* value) {
  prop_msg msg;

  if (key == 0) {
    return -1;
  }

  if (value == 0) {
    value = "";
  }


  if (strlen(key) >= PROP_NAME_MAX || strlen(value) >= PROP_VALUE_MAX) {
    LOG(ERROR) << "length of key or value is too long.";
    return -1;
  }

  msg.cmd = PROP_MSG_SETPROP;
  strcpy((char*) msg.name, key);
  strcpy((char*) msg.value, value);

  return send_prop_msg(&msg);
}

