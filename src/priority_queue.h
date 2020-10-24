#ifndef _priority_queue_h
#define _priority_queue_h

#include "container_tools.h"
#include "sort_tools.h"
#include "vector.h"

typedef struct PriorityQueue {
    Vector data;
    _compare_f compare;
    char *temp;
} PriorityQueue;

#define PriorityQueue_size(Q) Vector_size( &((Q)->data) )
#define PriorityQueue_empty(Q) Vector_empty( &((Q)->data) )
#define PriorityQueue_next(Q) ( (Q)->size ? (void*)((Q)->data.data[0]) : NULL )

int PriorityQueue_init(PriorityQueue *q,
                       const size_t element_size,
                       _destroy_f destroy,
                       _compare_f compare);
#define PriorityQueue_clear(Q) Vector_clear( &((Q)->data) )
void PriorityQueue_destroy(PriorityQueue *q);

int PriorityQueue_enqueue(PriorityQueue *q, const void *in);
int PriorityQueue_dequeue(PriorityQueue *q, void *out);

#endif // _priority_queue_h
