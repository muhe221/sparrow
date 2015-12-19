#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"

LogMessage::LogMessage(const int level, const char *file, const char *func,
    const int line, const int error) : level_(level), file_(file), func_(func), line_(line),
    error_(error) {
  const char* last_slash = strrchr(file, '/');
  file = (last_slash == NULL) ? file : last_slash + 1;
}


std::ostream& LogMessage::stream() {
  return buffer_;
}

void LogMessage::LogLine(const char* lineMsg) {
  char msg[LOG_BUFFER_SIZE + 2];
  pid_t pid = getpid();  //uinstd.h
  pthread_t thread_id = pthread_self();

  char tag;
  switch(level_) {
    case VERBOSE:
      tag = 'V';
      break;
    case INFO:
      tag = 'I';
      break;
    case DEBUG:
      tag = 'D';
      break;
    case WARN:
      tag = 'W';
      break;
    case ERROR:
      tag = 'E';
      break;
    case FATAL:
      tag = 'F';
      break;
    default:
      return;
  }

  sprintf(msg, "%d  %ld %c:  %s\n", pid, (long)thread_id, tag, lineMsg); 
  msg[LOG_BUFFER_SIZE] = '*';
  msg[LOG_BUFFER_SIZE + 1] = '\0';
//  printf("%s\n", msg);

  int size = strlen(msg);
  if (write(STDOUT_FILENO, msg, size) != size) {
    exit(1);
  }

}

LogMessage::~LogMessage() { 
  if (level_ < G_MIN_LOG_LEVEL) {
    return;
  }

  if (error_ != -1) {
    buffer_ << " error:" << strerror(error_);
  } 

  if (level_ >= WARN) {
#if 0
    char INFO[64];
    snprintf(INFO, 64, "file:%s, function:%s, line:%d", file_, func_, line_);
    LogLine(INFO);
#endif
    buffer_ << " file:" << file_ << ", function:" << func_ << ", line:" << line_;
  }

  std::string msg = buffer_.str(); 
  // TODO: log lock

  if (msg.find('\n') == std::string::npos) {
    LogLine(msg.c_str());
  } else {
    msg += '\n';
    size_t i = 0; 
    size_t nl;
    while (i < msg.size()) {
       nl = msg.find('\n', i);
       msg[nl] = '\0';
       LogLine(&msg[i]);
       i = nl + 1;
    }
  }

  if (level_ == FATAL) {
    abort(); // unistd.h
  }
}
