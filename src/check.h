/*************************************************************************************************
 *
 * check.h
 *
 * Various error checking macros.
 *
 * Author: Florian Kretlow, 2020, using patterns desribed by Zed A. Shaw.
 * License: MIT License
 *
 ************************************************************************************************/

#ifndef _check_h
#define _check_h

#include <errno.h>
#include <string.h>

#include "log.h"

#define check(T, fmt, ...) if (!(T)) { \
    log_error(fmt, ##__VA_ARGS__); \
    goto error; \
}

#define check_rc(rc, fn) if ((rc) < 0) { \
    log_error(fn " returned failure code %d", rc); \
    goto error; \
}

#define check_ptr(ptr) if ((ptr) == NULL) { \
    log_error("null pointer: %s", #ptr); \
    goto error; \
}

#define check_errno() if (errno != 0) { \
    log_error("%s", strerror(errno)); \
    errno = 0; \
    goto error; \
}

#define check_alloc(ptr) if ((ptr) == NULL && errno == ENOMEM) { \
    log_error("allocation failed: %s", strerror(errno)); \
    errno = 0; \
    goto error; \
}

#define sentinel(fmt, ...) log_error(fmt, ...); goto error

#endif // _check_h
