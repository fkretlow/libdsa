#ifndef _sort_h
#define _sort_h

#include <stdlib.h>

typedef void (*sort_f)(void* base, size_t nmemb, size_t size,
                       int (*compar)(const void*, const void*));

void quicksort(void* base, size_t nmemb, size_t size,
               int (*compar)(const void*, const void*));

int is_sorted(void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*));

#endif // _sort_h
