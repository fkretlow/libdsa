#ifndef _priority_queue_h
#define _priority_queue_h

#include "type_interface.h"
#include "vector.h"

typedef Vector PriorityQueue;

#define PriorityQueue_size(Q) Vector_size(Q)
#define PriorityQueue_empty(Q) Vector_empty(Q)
#define PriorityQueue_initialize(Q, element_type) Vector_initialize(Q, element_type)
#define PriorityQueue_destroy(Q) Vector_destroy(Q)
#define PriorityQueue_new(element_type) Vector_new(element_type)
#define PriorityQueue_delete(Q) Vector_delete(Q)
#define PriorityQueue_clear(Q) Vector_clear(Q)

int PriorityQueue_enqueue(PriorityQueue *Q, const void *in);
int PriorityQueue_dequeue(PriorityQueue *Q, void *out);

#endif // _priority_queue_h
