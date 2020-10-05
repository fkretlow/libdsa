#ifndef _quicksort_h
#define _quicksort_h

#include <stdlib.h>

void quicksort(void* base, size_t nmemb, size_t size,
               int (*compar)(const void*, const void*));

int is_sorted(void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*));

#endif //  _quicksort_h
