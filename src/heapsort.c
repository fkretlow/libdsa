#include <assert.h>
#include <stdlib.h>

#include "debug.h"
#include "sort_tools.h"

#define lchild(i) (2 * (i) + 1)
#define rchild(i) (2 * (i) + 2)
#define parent(i) (((i) - 1) / 2)

int __is_heap(char* base, size_t n, size_t size, __compare_f compare)
{
    if (n == 1) return 1;
    for (size_t i = 0; i <= parent(n - 1); ++i) {
        if (compare(base + i * size, base + lchild(i) * size) < 0 ||
            (rchild(i) < n && compare(base + i * size, base + rchild(i) * size) < 0)) {
            return 0;
        }
    }
    return 1;
}

static void __sift_down(char* base,
                        size_t n, size_t size, size_t i,
                        __compare_f compare,
                        char* temp)
{
    if (n == 1) return;
    size_t max;
    while (i <= parent(n - 1)) {
        max = i;
        if (compare(base + i * size, base + lchild(i) * size) < 0) {
            max = lchild(i);
        }
        if (rchild(i) < n && compare(base + max * size, base + rchild(i) * size) < 0) {
            max = rchild(i);
        }
        if (max == i) return;
        __swap(base + i * size, base + max * size, size, temp);
        i = max;
    }
}

static inline void __heapify(char* base,
                             size_t n, size_t size,
                             __compare_f compare,
                             char* temp)
{
    for (int i = parent((int)n - 1); i >= 0; --i) {
        __sift_down(base, n, size, (size_t)i, compare, temp);
    }
}

static inline void __heapsort(char* base,
                              size_t n, size_t size,
                              __compare_f compare,
                              char* temp)
{
    __heapify(base, n, size, compare, temp);
    assert(__is_heap(base, n, size, compare) && "heapsort >> initial __heapify failed.");

    for (size_t i = n - 1; i > 0; --i) {
        __swap(base, base + i * size, size, temp);
        __sift_down(base, --n, size, 0, compare, temp);
        assert(__is_heap(base, n, size, compare) && "heapsort >> no longer a heap.");
    }
}

void heapsort(void* base, size_t nmemb, size_t size, __compare_f compare)
{
    char* temp = malloc(size);
    __heapsort(base, nmemb, size, compare, temp);
    free(temp);
}
