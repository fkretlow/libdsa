#ifndef _debug_h
#define _debug_h

#include <stdio.h>

#define KRED "\x1B[31m"
#define KRESET "\x1B[0m"

int _suppress_errors;

#define check(T, M, ...) if (!(T)) { \
    if (!_suppress_errors) { \
        fprintf(stderr, KRED "[Error]" KRESET " %s:%d: %s: " M "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } \
    goto error; \
}

#define check_alloc(P) check((P), "Failed to allocate memory for " #P ".")
#define check_ptr(P) check((P), #P " is NULL.")

#define sentinel(M, ...) fprintf(stderr, KRED "[Error]" KRESET " %s:%d: %s: " M "\n", \
                                 __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    goto error;

#ifdef NDEBUG
    #define debug(M, ...)
#else
    #define debug(M, ...) fprintf(stderr, "[Debug] %s:%d: %s: " M "\n", \
                                  __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#endif

#define debug_array(M, A, n, ...) \
    fprintf(stderr, "Debug: " M, ##__VA_ARGS__); \
    for (size_t i = 0; i < (n); ++i) fprintf(stderr, "%d ", (A)[i]); \
    fprintf(stderr, "\n");

#endif // _debug_h
