#ifndef _queue_h
#define _queue_h

#include "list.h"

typedef List Queue;

#define Queue_init(Q, size, destroy) List_init(Q, size, destroy)
#define Queue_clear(Q) List_clear(Q)

#define Queue_push(Q, in) List_push_front(Q, in)
#define Queue_pop(Q, out) List_pop_back(Q, out)

#endif // _queue_h
