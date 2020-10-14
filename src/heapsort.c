#include <assert.h>
#include <stdlib.h>

#include "debug.h"
#include "heap.h"
#include "sort_tools.h"

static inline void __heapsort(char* base,
                              size_t n, size_t size,
                              __compare_f compare,
                              char* temp)
{
    make_heap(base, n, size, compare, temp);
    assert(is_heap(base, n, size, compare) && "heapsort >> initial __heapify failed.");

    for (size_t i = n - 1; i > 0; --i) {
        __swap(base, base + i * size, size, temp);
        --n;
        Heap_sift_down(base, n, size, 0, compare, temp);
        assert(is_heap(base, n, size, compare) && "heapsort >> no longer a heap.");
    }
}

void heapsort(void* base, size_t nmemb, size_t size, __compare_f compare)
{
    char* temp = malloc(size);
    __heapsort(base, nmemb, size, compare, temp);
    free(temp);
}
