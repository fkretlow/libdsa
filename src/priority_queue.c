#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "heap.h"
#include "priority_queue.h"
#include "sort_tools.h"

int PriorityQueue_init(PriorityQueue *q,
                       const size_t element_size,
                       _destroy_f destroy,
                       _compare_f compare)
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

void PriorityQueue_destroy(PriorityQueue *q)
{
    Vector_destroy(&(q->data));
    free(q->temp);
    q->temp = NULL;
}

int PriorityQueue_enqueue(PriorityQueue *q, const void *in)
{
    check_ptr(q);
    check_ptr(in);

    // Add the new element at the end.
    check(!Vector_push_back(&(q->data), in),
            "Failed to add value to underlying data vector.");

    // Move it upwards until the heap property is satisfied.
    if (Vector_size(&(q->data)) > 1) {
        Heap_bubble_up(q->data.data, q->data.element_size,
                       Vector_size(&(q->data)) - 1,
                       q->compare, q->temp);
        assert(is_heap(q->data.data, q->data.end, q->data.element_size, q->compare));
    }


    return 0;
error:
    return -1;
}

int PriorityQueue_dequeue(PriorityQueue *q, void *out)
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
        Heap_sift_down(q->data.data, q->data.end, q->data.element_size, 0,
                       q->compare, q->temp);
        assert(is_heap(q->data.data, q->data.end, q->data.element_size, q->compare));
    }


    return 0;
error:
    return -1;
}
