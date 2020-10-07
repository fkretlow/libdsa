#include <assert.h>
#include <stdlib.h>

#include "debug.h"
#include "sort_tools.h"

#define lchild(i) (2 * (i) + 1)
#define rchild(i) (2 * (i) + 2)
#define parent(i) (((i) - 1) / 2)

int temp;
#define swap(a,b) temp = a; a = b; b = temp;

int is_heap(int* A, size_t n)
{
    for (size_t i = 0; i <= parent(n - 1); ++i) {
        if (A[i] < A[lchild(i)] || rchild(i) < n && A[i] < A[rchild(i)]) return 0;
    }
    return 1;
}

static void __sift_down(int* A, size_t n, size_t i)
{
    if (n == 1) return;
    size_t max;
    while (i <= parent(n - 1)) {
        max = i;
        if (A[i] < A[lchild(i)]) max = lchild(i);
        if (rchild(i) < n && A[max] < A[rchild(i)]) max = rchild(i);
        if (max == i) return;
        swap(A[i], A[max]);
        i = max;
    }
}

void __heapify(int* A, size_t n)
{
    for (int i = parent((int)n - 1); i >= 0; --i) {
        __sift_down(A, n, (size_t)i);
    }
}

void heapsort(int* A, size_t n)
{
    __heapify(A, n);
    assert(is_heap(A, n) && "heapsort >> __heapify failed.");

    for (size_t i = n - 1; i > 0; --i) {
        swap(A[0], A[i]);
        __sift_down(A, --n, 0);
    }
}
