/*************************************************************************************************
 *
 * heap.c
 *
 * Implementation of the heap functions defined in heap.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <stdlib.h>
#include "heap.h"

#define _lchild(i) (2 * (i) + 1)
#define _rchild(i) (2 * (i) + 2)
#define _parent(i) (((i) - 1) / 2)

/* Test if the order of the elements in the given array satisfies the heap property. */
int is_heap(char *base, const size_t n, const size_t size, compare_f compare)
{
    if (n > 1) {
        for (size_t i = 0; i <= _parent(n - 1); ++i) {
            if (compare(base + i * size, base + _lchild(i) * size) < 0 ||
                (_rchild(i) < n && compare(base + i * size, base + _rchild(i) * size) < 0)) {
                return 0;
            }
        }
    }
    return 1;
}

/* Rearrange the elements in the given array in such a way that they satisfy the heap property. */
void make_heap(char *base, const size_t n, const size_t size,
               compare_f compare,
               char *temp)
{
    for (int i = _parent((int)n - 1); i >= 0; --i) {
        heap_sift_down(base, n, size, (size_t)i, compare, temp);
    }
}

/* Make the element at index i move up the heap until it sits where it belongs. Used to repair the
 * heap after inserting an element at the end of the array. */
void heap_bubble_up(char *base, const size_t size,
                    size_t i,
                    compare_f compare,
                    char *temp)
{
    size_t p = _parent(i);
    while (i > 0 && compare(base + i * size, base + p * size) > 0) {
        _swap(base + i * size, base + p * size, size, temp);
        i = p;
        p = _parent(p);
    }
}

/* Make the element at index i move down the heap until it sits where it belongs. Used by
 * make_heap. */
void heap_sift_down(char *base, const size_t n, const size_t size,
                    size_t i,
                    compare_f compare,
                    char *temp)
{
    if (n == 1) return;
    size_t max;
    while (i <= _parent(n - 1)) {
        max = i;
        if (compare(base + i * size, base + _lchild(i) * size) < 0) {
            max = _lchild(i);
        }
        if (_rchild(i) < n && compare(base + max * size, base + _rchild(i) * size) < 0) {
            max = _rchild(i);
        }
        if (max == i) return;
        _swap(base + i * size, base + max * size, size, temp);
        i = max;
    }
}
