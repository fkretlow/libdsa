#include <stdlib.h>

#include "log.h"
#include "priority_queue.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define MAXV 1000

static pqueue *Q;
static int rc, v1, v2;

int test_pqueue_new(void)
{
    Q = pqueue_new(&int_type);
    return 0;
}

int test_pqueue_usage(void)
{
    for (int i = 0; i < 32; ++i) {
        v1 = rand() % MAXV;
        rc = pqueue_enqueue(Q, &v1);
        test(rc == 1);
    }

    rc = pqueue_dequeue(Q, &v1);
    test(rc == 1);

    while (pqueue_count(Q) > 0) {
        rc = pqueue_dequeue(Q, &v2);
        test(rc == 1);
        test(v1 >= v2);
        v1 = v2;
    }

    return 0;
}

int test_pqueue_teardown(void)
{
    pqueue_clear(Q);
    test(Q->count == 0);
    pqueue_delete(Q);
    return 0;
}

int main(void)
{
    srand(1);

    test_suite_start();
    run_test(test_pqueue_new);
    run_test(test_pqueue_usage);
    run_test(test_pqueue_teardown);
    test_suite_end();
}
