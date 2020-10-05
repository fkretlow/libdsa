#ifndef _debug_h
#define _debug_h

#define debug(M, ...) fprintf(stderr, M "\n", ##__VA_ARGS__);

#define check(T, M, ...) if (!(T)) { \
    fprintf(stderr, "Error: " M "\n", ##__VA_ARGS__); \
    goto error; \
    }

#endif // _debug_h
