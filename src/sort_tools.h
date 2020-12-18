#ifndef _sort_tools_h
#define _sort_tools_h

#include <stddef.h>
#include <string.h>

typedef int (*compare_f)(const void *a, const void *b);

static inline void _swap(char *a, char *b, size_t size, char *temp)
{
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

void _insertionsort(char *base,
                    size_t start, size_t end, size_t size,
                    compare_f compare,
                    char *temp);

#endif /* _sort_tools_h */
