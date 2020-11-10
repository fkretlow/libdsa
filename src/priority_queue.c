#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "heap.h"
#include "priority_queue.h"

int PriorityQueue_initialize(PriorityQueue *Q, TypeInterface *element_type)
{
    check_ptr(element_type);
    check(element_type->compare != NULL, "Need a comparison function.")

    check(!Vector_initialize(&Q->vector, element_type),
            "Failed to initialize vector for priority queue data.");
    Q->temp = malloc(element_type->size);
    check_alloc(Q->temp);

    return 0;
error:
    return -1;
}

PriorityQueue *PriorityQueue_new(TypeInterface *element_type)
{
    PriorityQueue *Q = malloc(sizeof(*Q));
    check_alloc(Q);

    int rc = PriorityQueue_initialize(Q, element_type);
    check(rc == 0, "Failed to initialize new priority queue.");

    return Q;
error:
    if (Q->vector.data) Vector_deallocate(&Q->vector);
    if (Q->temp) free(Q->temp);
    free(Q);
    return NULL;
}

void PriorityQueue_delete(PriorityQueue *Q)
{
    if (Q) {
        Vector_deallocate(&Q->vector);
        free(Q->temp);
        free(Q);
    }
}

int PriorityQueue_enqueue(PriorityQueue *Q, const void *in)
{
    check_ptr(Q);
    check_ptr(in);

    /* Add the new element at the end. */
    check(!Vector_push_back(&Q->vector, in),
            "Failed to add value to underlying data vector.");

    /* Move it upwards until the heap property is satisfied. */
    if (Vector_size(&Q->vector) > 1) {
        Heap_bubble_up(Q->vector.data, TypeInterface_size(Q->vector.element_type),
                       Vector_size(&Q->vector) - 1,
                       Q->vector.element_type->compare, Q->temp);
        assert(is_heap(Q->vector.data, Q->vector.size,
                       TypeInterface_size(Q->vector.element_type),
                       Q->vector.element_type->compare));
    }

    return 0;
error:
    return -1;
}

int PriorityQueue_dequeue(PriorityQueue *Q, void *out)
{
    check_ptr(Q);
    check_ptr(out);

    check(!Vector_get(&Q->vector, 0, out), "Failed to hand out value.");

    /* Copy the last element to the top, assuming that nested types remain
     * intact when only the top level data is moved. */
    memmove(Q->vector.data,
            Q->vector.data + (Q->vector.size - 1) * TypeInterface_size(Q->vector.element_type),
            TypeInterface_size(Q->vector.element_type));

    /* Remove the now obsolete duplicate at the end. */
    check(!Vector_pop_back(&Q->vector, NULL), "Failed to remove value.");

    /* Repair the heap. */
    if (Vector_size(&Q->vector) > 1) {
        Heap_sift_down(Q->vector.data, Q->vector.size,
                       TypeInterface_size(Q->vector.element_type), 0,
                       Q->vector.element_type->compare, Q->temp);
        assert(is_heap(Q->vector.data, Q->vector.size,
                       TypeInterface_size(Q->vector.element_type),
                       Q->vector.element_type->compare));
    }


    return 0;
error:
    return -1;
}
