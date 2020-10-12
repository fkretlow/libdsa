#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "priority_queue.h"
#include "sort_tools.h"

#define lchild(i) (2 * (i) + 1)
#define rchild(i) (2 * (i) + 2)
#define parent(i) (((i) - 1) / 2)

static int __is_heap(char* base, size_t n, size_t size, __comparison_f compare)
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

static void __Heap_bubble_up(char* base,
                             const size_t size, size_t i,
                             __comparison_f compare,
                             char* temp)
{
    size_t p = parent(i);
    while (i > 0 && compare(base + i * size, base + p * size) > 0) {
        __swap(base + i * size, base + p * size, size, temp);
        i = p;
        p = parent(p);
    }
}

static void __Heap_sift_down(char* base,
                             size_t n, size_t size, size_t i,
                             __comparison_f compare,
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

int PriorityQueue_init(PriorityQueue* q,
                       const size_t element_size,
                       __destroy_f destroy,
                       __comparison_f compare)
{
    check(!Vector_init(&(q->data), element_size, destroy),
            "Failed to initialize vector for priority queue data.");
    q->compare = compare;
    q->temp = malloc(element_size);
    check_alloc(q->temp);

    return 0;
error:
    if (q->data.data) Vector_destroy(&(q->data));
    if (q->temp) free(q->temp);
    return -1;
}

void PriorityQueue_destroy(PriorityQueue* q)
{
    Vector_destroy(&(q->data));
    free(q->temp);
    q->temp = NULL;
}

int PriorityQueue_push(PriorityQueue* q, const void* in)
{
    check_ptr(q);
    check_ptr(in);

    // Add the new element at the end.
    check(!Vector_push_back(&(q->data), in),
            "Failed to add value to underlying data vector.");

    // Move it upwards until the heap property is satisfied.
    if (Vector_size(&(q->data)) > 1) {
        __Heap_bubble_up(q->data.data,
                         q->data.element_size,
                         Vector_size(&(q->data)) - 1,
                         q->compare,
                         q->temp);
        assert(__is_heap(q->data.data, q->data.end, q->data.element_size, q->compare));
    }


    return 0;
error:
    return -1;
}

int PriorityQueue_pop(PriorityQueue* q, void* out)
{
    check_ptr(q);
    check_ptr(out);

    check(!Vector_get(&(q->data), 0, out), "Failed to hand out value.");

    // Copy the last element to the top.
    memmove(q->data.data,
            q->data.data + (q->data.end - 1) * q->data.element_size,
            q->data.element_size);

    // Remove the now obsolete duplicate at the end.
    check(!Vector_pop_back(&(q->data), NULL), "Failed to remove value.");

    // Repair the heap.
    if (Vector_size(&(q->data)) > 1) {
        __Heap_sift_down(q->data.data,
                         q->data.end, q->data.element_size, 0,
                         q->compare, q->temp);
        assert(__is_heap(q->data.data, q->data.end, q->data.element_size, q->compare));
    }


    return 0;
error:
    return -1;
}
