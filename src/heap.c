#include <assert.h>
#include <stdlib.h>

#include "container_tools.h"
#include "heap.h"
#include "sort_tools.h"

#define __lchild(i) (2 * (i) + 1)
#define __rchild(i) (2 * (i) + 2)
#define __parent(i) (((i) - 1) / 2)

int is_heap(char* base, const size_t n, const size_t size, __compare_f compare)
{
    if (n > 1) {
        for (size_t i = 0; i <= __parent(n - 1); ++i) {
            if (compare(base + i * size, base + __lchild(i) * size) < 0 ||
                (__rchild(i) < n && compare(base + i * size, base + __rchild(i) * size) < 0)) {
                return 0;
            }
        }
    }
    return 1;
}

void make_heap(char* base, const size_t n, const size_t size,
               __compare_f compare,
               char* temp)
{
    for (int i = __parent((int)n - 1); i >= 0; --i) {
        Heap_sift_down(base, n, size, (size_t)i, compare, temp);
    }
    assert(is_heap(base, n, size, compare));
}

void Heap_bubble_up(char* base, const size_t size,
                    size_t i,
                    __compare_f compare,
                    char* temp)
{
    size_t p = __parent(i);
    while (i > 0 && compare(base + i * size, base + p * size) > 0) {
        __swap(base + i * size, base + p * size, size, temp);
        i = p;
        p = __parent(p);
    }
}

void Heap_sift_down(char* base, const size_t n, const size_t size,
                    size_t i,
                    __compare_f compare,
                    char* temp)
{
    if (n == 1) return;
    size_t max;
    while (i <= __parent(n - 1)) {
        max = i;
        if (compare(base + i * size, base + __lchild(i) * size) < 0) {
            max = __lchild(i);
        }
        if (__rchild(i) < n && compare(base + max * size, base + __rchild(i) * size) < 0) {
            max = __rchild(i);
        }
        if (max == i) return;
        __swap(base + i * size, base + max * size, size, temp);
        i = max;
    }
}
