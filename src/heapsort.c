/*************************************************************************************************
 *
 * heapsort.c
 * Implementation of the heapsort algorithm. Basically "push everything into a priority queue and
 * then pop one by one". See heap.h/c for details. Sources: Skiena, Wikipedia.
 *
 ************************************************************************************************/

#include <stdlib.h>

#include "check.h"
#include "heap.h"
#include "sort_tools.h"

void heapsort(char *base, size_t n, size_t size, compare_f compare)
{
    char *temp = malloc(size);
    check_alloc(temp);

    make_heap(base, n, size, compare, temp);

    for (size_t i = n - 1; i > 0; --i) {
        _swap(base, base + i * size, size, temp);
        --n;
        heap_sift_down(base, n, size, 0, compare, temp);
    }

    free(temp);
error:
    return; /* TODO: error handling?? */
}
