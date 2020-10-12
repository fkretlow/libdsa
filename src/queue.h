#ifndef _queue_h
#define _queue_h

#include "list.h"

typedef List Queue;

#define Queue_next(Q) List_last(Q)
#define Queue_size(Q) List_size(Q)
#define Queue_empty(Q) List_empty(Q)

#define Queue_init(Q, size, destroy) List_init(Q, size, destroy)
#define Queue_clear(Q) List_clear(Q)

#define Queue_push(Q, in) List_push_front(Q, in)
#define Queue_pop(Q, out) List_pop_back(Q, out)

#endif // _queue_h
