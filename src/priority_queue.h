/*************************************************************************************************
 *
 * priority_queue.h
 *
 * Declaration of the priority queue abstraction. See also vector.h and heap.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _priority_queue_h
#define _priority_queue_h

#include "type_interface.h"
#include "vector.h"

typedef vector pqueue;

#define pqueue_count(Q)             vector_count(Q)
#define pqueue_empty(Q)             vector_empty(Q)
#define pqueue_initialize(Q, dt)    vector_initialize(Q, dt)
#define pqueue_destroy(Q)           vector_destroy(Q)
#define pqueue_new(dt)              vector_new(dt)
#define pqueue_delete(Q)            vector_delete(Q)
#define pqueue_clear(Q)             vector_clear(Q)

int pqueue_enqueue(pqueue *Q, const void *in);
int pqueue_dequeue(pqueue *Q, void *out);

#endif /* _priority_queue_h */
