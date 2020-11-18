#include <stdlib.h>

#include "log.h"
#include "priority_queue.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define MAX_VAL 1000

static PriorityQueue *Q;
static int rc, v1, v2;

int test_priority_queue_new(void)
{
    Q = PriorityQueue_new(&int_type);
    return 0;
}

int test_priority_queue_usage(void)
{
    for (int i = 0; i < 32; ++i) {
        v1 = rand() % MAX_VAL;
        rc = PriorityQueue_enqueue(Q, &v1);
        test(rc == 0);
    }

    rc = PriorityQueue_dequeue(Q, &v1);
    test(rc == 0);

    for (int i = 0; PriorityQueue_size(Q) > 0; ++i) {
        rc = PriorityQueue_dequeue(Q, &v2);
        test(rc == 0);
        test(v1 >= v2);
        v1 = v2;
    }

    return 0;
}

int test_priority_queue_teardown(void)
{
    PriorityQueue_clear(Q);
    test(Q->size == 0);
    PriorityQueue_delete(Q);
    return 0;
}

int main(void)
{
    srand(1);

    test_suite_start();
    run_test(test_priority_queue_new);
    run_test(test_priority_queue_usage);
    run_test(test_priority_queue_teardown);
    test_suite_end();
}
