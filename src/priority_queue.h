#ifndef _priority_queue_h
#define _priority_queue_h

#include "type_interface.h"
#include "vector.h"

typedef struct PriorityQueue {
    Vector vector;
    char *temp;
} PriorityQueue;

#define PriorityQueue_size(Q) Vector_size( &(Q)->vector )
#define PriorityQueue_empty(Q) Vector_empty( &(Q)->vector )
#define PriorityQueue_next(Q) ( (Q)->size ? (void*)((Q)->vector.data[0]) : NULL )

int PriorityQueue_initialize(PriorityQueue *Q, TypeInterface *element_type);
PriorityQueue *PriorityQueue_new(TypeInterface *element_type);
void PriorityQueue_delete(PriorityQueue *Q);
#define PriorityQueue_clear(Q) Vector_clear( &(Q)->vector )

int PriorityQueue_enqueue(PriorityQueue *Q, const void *in);
int PriorityQueue_dequeue(PriorityQueue *Q, void *out);

#endif // _priority_queue_h
