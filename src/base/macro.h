#ifndef SPARROW_MACRO_H
#define SPARROW_MACRO_H

#include <errno.h>

#define CHECK_EQ(X, Y) \
  do { \
    if (X != Y) { \
      LOG(FATAL) << "ASSERTION FAILURE"; \
    } \
  } while (false)


#define CHECK_PTHREAD_CALL(call, args, what) \
  do { \
    int rc = call args; \
    if (rc != 0) { \
      errno = rc; \
      LOG(FATAL) << # call << " failed for " << what; \
    } \
  } while (false)


#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp) ({         \
    typeof (exp) _rc;                      \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })
#endif

#endif
