#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "heap.h"
#include "priority_queue.h"
#include "sort_tools.h"

PriorityQueue PriorityQueue_new(const size_t element_size,
                      copy_f copy_element,
                      destroy_f destroy_element,
                      compare_f compare)
{
    _pqueue *Q = malloc(sizeof(*Q));
    check_alloc(Q);

    check(!_vector_init(&(Q->vector), element_size, copy_element, destroy_element),
            "Failed to initialize vector for priority queue data.");
    Q->compare = compare;
    Q->temp = malloc(element_size);
    check_alloc(Q->temp);

    return Q;
error:
    if (Q->vector.data) _vector_dealloc(&(Q->vector));
    if (Q->temp) free(Q->temp);
    free(Q);
    return NULL;
}

void PriorityQueue_delete(PriorityQueue Q)
{
    if (Q) {
        _vector_dealloc(&(Q->vector));
        free(Q->temp);
        free(Q);
    }
}

int PriorityQueue_enqueue(PriorityQueue Q, const void *in)
{
    check_ptr(Q);
    check_ptr(in);

    /* Add the new element at the end. */
    check(!Vector_push_back(&(Q->vector), in),
            "Failed to add value to underlying data vector.");

    /* Move it upwards until the heap property is satisfied. */
    if (Vector_size(&(Q->vector)) > 1) {
        Heap_bubble_up(Q->vector.data, Q->vector.element_size,
                       Vector_size(&(Q->vector)) - 1,
                       Q->compare, Q->temp);
        assert(is_heap(Q->vector.data, Q->vector.size, Q->vector.element_size,
                       Q->compare));
    }

    return 0;
error:
    return -1;
}

int PriorityQueue_dequeue(PriorityQueue Q, void *out)
{
    check_ptr(Q);
    check_ptr(out);

    check(!Vector_get(&(Q->vector), 0, out), "Failed to hand out value.");

    /* Copy the last element to the top, assuming that nested types remain
     * intact when only the top level data is moved. */
    memmove(Q->vector.data,
            Q->vector.data + (Q->vector.size - 1) * Q->vector.element_size,
            Q->vector.element_size);

    /* Remove the now obsolete duplicate at the end. */
    check(!Vector_pop_back(&(Q->vector), NULL), "Failed to remove value.");

    /* Repair the heap. */
    if (Vector_size(&(Q->vector)) > 1) {
        Heap_sift_down(Q->vector.data, Q->vector.size, Q->vector.element_size, 0,
                       Q->compare, Q->temp);
        assert(is_heap(Q->vector.data, Q->vector.size, Q->vector.element_size,
                       Q->compare));
    }


    return 0;
error:
    return -1;
}
