/*************************************************************************************************
 *
 * quicksort.c
 * Implementation of the quicksort algorithm. Sources: Skiena, Wikipedia.
 *
 ************************************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "sort_tools.h"

static size_t _partition(char *base,
                         size_t start, size_t end,
                         size_t size,
                         compare_f compare,
                         char *temp)
{
    // Lomuto partitioning scheme
    /* size_t first_high, pivot;

    first_high = start;
    pivot = end - 1;

    for (size_t i = start; i < pivot; ++i) {
        if (compare(base + i*size, base + pivot*size) < 0) {
            _swap((base + i*size), (base + first_high*size), size, temp);
            ++first_high;
        }
    }
    _swap((base + first_high*size), (base + pivot*size), size, temp);

    return first_high; */

    // Hoare partitioning scheme
    size_t i = start - 1;
    size_t j = end;

    /* It's important to get the pivot position right to guarantee it's not at the last index in
     * the range. Otherwise the upper partition could end up empty, which would lead to infinite
     * recursion. */
    size_t p = (end - 1 + start) / 2;

    /* Copy the pivot value to the allocated workspace because it may be moved and it's ugly (and
     * less efficient?) to keep track of it. */
    char *pivot = temp + size;
    memcpy(pivot, base + p * size, size);

    for ( ;; ) {
        do {
            ++i;
        } while (compare(base + i * size, pivot) < 0);
        do {
            --j;
        } while (compare(base + j * size, pivot) > 0);

        /* At this point we have a few invariants:
         * 1. A[k] <= pivot if k < i and A[k] >= pivot if k > j.
         * 2. j >= i - 1. If j = i then A[j] = pivot. If j = i - 1 then A[j] <= pivot.
         * 3. start <= j < end - 1.
         * Assuming end - start > 1, these guarantee that we partition the range in two non-empty
         * partitions [start, j] (elements <= pivot) and ]j, end[ (elements >= pivot). */
        if (i >= j) {
            return j;
        } else {
            _swap(base + i * size, base + j * size, size, temp);
        }
    }
}

static void _quicksort(char *base,
                        size_t start, size_t end,
                        size_t size,
                        compare_f compare,
                        char *temp)
{
    if (end - start <= 1) {
        return;
    } else if (end - start <= 16) {
        /* Using insertion sort for short ranges seems to give a significant speed boost of about
         * 10-15%. */
        _insertionsort(base, start, end, size, compare, temp);
    } else {
        size_t p = _partition(base, start, end, size, compare, temp);
        // Lomuto partitioning scheme
        /* _quicksort(base, start, p, size, compare, temp);
        _quicksort(base, p + 1, end, size, compare, temp); */

        // Hoare partitioning scheme
        _quicksort(base, start, p + 1, size, compare, temp);
        _quicksort(base, p + 1, end, size, compare, temp);
    }
}

void quicksort(void *base, size_t nmemb, size_t size, compare_f compare) {
    // Allocate workspace for swaps and the pivot value.
    char *temp = malloc(2 * size);
    check_alloc(temp);
    _quicksort((char*)base, 0, nmemb, size, compare, temp);
    free(temp);
error:
    return; /* TODO: error handling?? */
}

int is_sorted(void *base, size_t nmemb, size_t size, compare_f compare)
{
    for (size_t i = 0; i < nmemb - 1; ++i) {
        if (compare(base + i*size, base + (i+1)*size) > 0) return 0;
    }
    return 1;
}
