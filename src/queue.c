#include "check.h"
#include "log.h"
#include "queue.h"

int queue_dequeue(queue *Q, void *out)
{
    log_call("Q=%p, out=%p", Q, out);
    check_ptr(Q);

    if (Q->count == 0) return 0;

    if (out) {
        t_move(Q->data_type, out, listn_data(Q->last));
        Q->last->has_data = 0;
    }

    return list_pop_back(Q);
error:
    return -1;
}
