#ifndef _heap_h
#define _heap_h

#include "sort_tools.h"

int is_heap(char *base, const size_t n, const size_t size, __compare_f compare);

void make_heap(char *base, const size_t n, const size_t size,
               __compare_f compare,
               char *temp);

void Heap_bubble_up(char *base, const size_t size,
                    size_t i,
                    __compare_f compare,
                    char *temp);

void Heap_sift_down(char *base, const size_t n, const size_t size,
                    size_t i,
                    __compare_f compare,
                    char *temp);

#endif // _heap_h
