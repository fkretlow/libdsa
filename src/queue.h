/*************************************************************************************************
 *
 * queue.h
 * Simple queue (FIFO), just an adapter for a list.
 *
 ************************************************************************************************/

#ifndef _queue_h
#define _queue_h

#include "list.h"

typedef list queue;

#define queue_next(Q)           list_last(Q)
#define queue_count(Q)          list_count(Q)
#define queue_empty(Q)          list_empty(Q)

#define queue_initialize(Q, dt) list_initialize(dt)
#define queue_new(dt)           list_new(dt)
#define queue_delete(Q)         list_delete(Q)
#define queue_clear(Q)          list_clear(Q)

#define queue_enqueue(Q, in)    list_push_front(Q, in)
#define queue_dequeue(Q, out)   list_pop_back(Q, out)

#endif // _queue_h
