#include <stdlib.h>
#include <string.h>

#include "sort_tools.h"

void __swap(char* a, char* b, size_t size, char* temp)
{
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

void __insertionsort(char *base,
                     size_t start, size_t end,
                     size_t size,
                     __comparison_f compare,
                     char* temp)
{
    size_t i, j;
    for (i = start + 1; i < end; ++i) {
        j = i;
        while (j > start && compare(base + j * size, base + (j - 1) * size) < 0) {
            __swap(base + j * size, base + (j - 1) * size, size, temp);
            --j;
        }
    }
}
