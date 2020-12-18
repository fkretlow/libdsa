#include <assert.h>
#include <stdlib.h>

#include "heap.h"
#include "sort_tools.h"

void heapsort(char *base, size_t n, size_t size, compare_f compare)
{
    char *temp = malloc(size);

    make_heap(base, n, size, compare, temp);
    assert(is_heap(base, n, size, compare) && "heapsort >> initial _heapify failed.");

    for (size_t i = n - 1; i > 0; --i) {
        _swap(base, base + i * size, size, temp);
        --n;
        heap_sift_down(base, n, size, 0, compare, temp);
        assert(is_heap(base, n, size, compare) && "heapsort >> no longer a heap.");
    }

    free(temp);
}
