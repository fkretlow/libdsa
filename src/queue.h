#ifndef _queue_h
#define _queue_h

#include "list.h"

typedef list Queue;

#define Queue_next(Q) list_last(Q)
#define Queue_count(Q) list_count(Q)
#define Queue_empty(Q) list_empty(Q)

#define Queue_initialize(Q, element_type) list_initialize(element_type)
#define Queue_new(element_type) list_new(element_type)
#define Queue_delete(Q) list_delete(Q)
#define Queue_clear(Q) list_clear(Q)

#define Queue_enqueue(Q, in) list_push_front(Q, in)
#define Queue_dequeue(Q, out) list_pop_back(Q, out)

#endif // _queue_h
