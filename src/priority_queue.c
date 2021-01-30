/*************************************************************************************************
 *
 * priority_queue.c
 *
 * Implementation of the priority queue abstraction in terms of a heap on a vector. See also
 * vector.h and heap.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "check.h"
#include "heap.h"
#include "priority_queue.h"

/* int pqueue_enqueue(pqueue *Q, const void *in)
 * Add the item at in to the queue. Return 1 if it was successfully added, or -1 on error. */
int pqueue_enqueue(pqueue *Q, const void *in)
{
    char *temp = NULL;
    check_ptr(Q);
    check_ptr(in);

    /* Add the new element at the end. */
    vector_push_back(Q, in);

    /* Move it upwards until the heap property is satisfied. */
    if (pqueue_count(Q) > 1) {
        temp = malloc(t_size(Q->data_type));
        check_alloc(temp);
        heap_bubble_up(Q->data, t_size(Q->data_type),
                       Q->count - 1, Q->data_type->compare, temp);
        assert(is_heap(Q->data, Q->count,
                       t_size(Q->data_type),
                       Q->data_type->compare));
        free(temp);
    }

    return 1;
error:
    if (temp) free(temp);
    return -1;
}

/* int pqueue_dequeue(pqueue *Q, void *out);
 * Remove the next item in the queue, store it at out (assuming sufficient memory) unless out is
 * NULL. Return 1 if an item was removed, 0 if the queue was empty, or -1 on error. */
int pqueue_dequeue(pqueue *Q, void *out)
{
    char *temp = NULL;
    check_ptr(Q);
    check_ptr(out);

    if (Q->count == 0) return 0;

    if (out) t_move(Q->data_type, out, vector_first(Q));
    else t_destroy(Q->data_type, vector_first(Q));

    /* move the last element to the top */
    t_move(Q->data_type, vector_first(Q), vector_last(Q));
    --Q->count;
    if (Q->count < (Q->capacity >> 2) && Q->capacity > VECTOR_MIN_CAPACITY) {
        int rc = vector_shrink_to_fit(Q);
        if (rc < 0) log_warn("failed to contract internal storage");
    }

    /* Repair the heap. */
    if (Q->count > 1) {
        temp = malloc(t_size(Q->data_type));
        check_alloc(temp);
        heap_sift_down(Q->data, Q->count,
                       t_size(Q->data_type), 0,
                       Q->data_type->compare, temp);
        assert(is_heap(Q->data, Q->count,
                       t_size(Q->data_type),
                       Q->data_type->compare));
        free(temp);
    }

    return 1;
error:
    if (temp) free(temp);
    return -1;
}
