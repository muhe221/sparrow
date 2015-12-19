#ifndef SPARROW_LOG_H
#define SPARROW_LOG_H

#include <sstream>
#include <errno.h>

#include "config.h"

using std::ostringstream;

#define LOG_MAX_LENGTH  512
#define LOG_MIN_LENGTH  8

const int VERBOSE = 0;
const int INFO = 1;
const int DEBUG = 2;
const int WARN = 3;
const int ERROR = 4;
const int FATAL = 5;

class LogMessage {
public:
  LogMessage(const int level, const char *file, const char *func, const int line, const int error);
  ~LogMessage();

  std::ostream& stream();

private:
  void LogLine(const char* lineMsg);

private:
  const int level_;
  const char *file_;
  const char *func_;
  const int line_;
  const int error_;

  std::ostringstream buffer_; 
};

#define LOG(level)   LogMessage(level, __FILE__, __func__, __LINE__, -1).stream() 
#define PLOG(level)  LogMessage(level, __FILE__, __func__, __LINE__, errno).stream()

#endif
