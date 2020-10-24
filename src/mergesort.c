#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "sort_tools.h"

static void _merge(char *base,
                    size_t start, size_t end, size_t middle,
                    size_t size,
                    _compare_f compare,
                    char *temp)
{
    // Move the first half to the workspace.
    memmove(temp + start * size, base + start * size, (middle - start) * size);

    size_t i, j, k;
    for (i = start, j = middle, k = start; i < middle && j < end; ++k) {
        if (compare(temp + i * size, base + j * size) <= 0) {
            memmove(base + k * size, temp + i * size, size);
            ++i;
        } else {
            memmove(base + k * size, base + j * size, size);
            ++j;
        }
    }

    while (i < middle) {
        memmove(base + k * size, temp + i * size, size);
        ++i; ++k;
    }
    // No need to finish walking through the second half if j < end,
    // because the values are already at their place.
}

static void _mergesort(char *base,
                        size_t start, size_t end,
                        size_t size,
                        _compare_f compare,
                        char *temp)
{
    if (end - start <= 1) {
        return;
    } else if (end - start <= 8) {
        _insertionsort(base, start, end, size, compare, temp);
    } else {
        size_t middle = (start + end) / 2;
        _mergesort(base, start, middle, size, compare, temp);
        _mergesort(base, middle, end, size, compare, temp);
        _merge(base, start, end, middle, size, compare, temp);
    }
}

void mergesort(void *base, size_t nmemb, size_t size, _compare_f compare)
{
    char *temp = malloc(nmemb * size);
    _mergesort((char*)base, 0, nmemb, size, compare, temp);
    free(temp);
}
