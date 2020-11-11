#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "heap.h"
#include "priority_queue.h"

int PriorityQueue_enqueue(PriorityQueue *Q, const void *in)
{
    char *temp = NULL;
    check_ptr(Q);
    check_ptr(in);

    /* Add the new element at the end. */
    Vector_push_back(Q, in);

    /* Move it upwards until the heap property is satisfied. */
    if (PriorityQueue_size(Q) > 1) {
        temp = malloc(TypeInterface_size(Q->element_type));
        check_alloc(temp);
        Heap_bubble_up(Q->data, TypeInterface_size(Q->element_type),
                       Q->size - 1, Q->element_type->compare, temp);
        assert(is_heap(Q->data, Q->size,
                       TypeInterface_size(Q->element_type),
                       Q->element_type->compare));
        free(temp);
    }

    return 0;
error:
    if (temp) free(temp);
    return -1;
}

int PriorityQueue_dequeue(PriorityQueue *Q, void *out)
{
    char *temp = NULL;
    check_ptr(Q);
    check_ptr(out);

    Vector_get(Q, 0, out);

    /* Copy the last element to the top, assuming that nested types remain
     * intact when only the top level data is moved. */
    memmove(Q->data,
            Q->data + (Q->size - 1) * TypeInterface_size(Q->element_type),
            TypeInterface_size(Q->element_type));

    /* Remove the now obsolete duplicate at the end. */
    Vector_pop_back(Q, NULL);

    /* Repair the heap. */
    if (Q->size > 1) {
        temp = malloc(TypeInterface_size(Q->element_type));
        check_alloc(temp);
        Heap_sift_down(Q->data, Q->size,
                       TypeInterface_size(Q->element_type), 0,
                       Q->element_type->compare, temp);
        assert(is_heap(Q->data, Q->size,
                       TypeInterface_size(Q->element_type),
                       Q->element_type->compare));
        free(temp);
    }

    return 0;
error:
    if (temp) free(temp);
    return -1;
}
