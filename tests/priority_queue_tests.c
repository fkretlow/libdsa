#include <stdlib.h>

#include "debug.h"
#include "priority_queue.h"
#include "test_utils.h"
#include "unittest.h"

#define MAX_VAL 1000

static PriorityQueue Q;
static int rc, v1, v2;

int test_priority_queue_init(void)
{
    rc = PriorityQueue_init(&Q, sizeof(int), NULL, compint);
    return TEST_OK;
}

int test_priority_queue_usage(void)
{
    for (int i = 0; i < 32; ++i) {
        v1 = rand() % MAX_VAL;
        rc = PriorityQueue_push(&Q, &v1);
        test(rc == 0, "PriorityQueue_push failed (for-loop i=%d)", i);
    }

    rc = PriorityQueue_pop(&Q, &v1);
    test(rc == 0, "PriorityQueue_pop failed.");

    for (int i = 0; PriorityQueue_size(&Q) > 0; ++i) {
        rc = PriorityQueue_pop(&Q, &v2);
        test(rc == 0, "PriorityQueue_pop failed (for-loop i=%d)", i);
        test(v1 >= v2, "v1=%d, v2=%d, v1<v2", v1, v2);
        v1 = v2;
    }

    return TEST_OK;
}

int test_priority_queue_clear(void)
{
    PriorityQueue_clear(&Q);
    test(Q.data.end == 0, "Q.data.end = %lu (%lu)", Q.data.end, 0lu);
    return TEST_OK;
}

int test_priority_queue_destroy(void)
{
    PriorityQueue_destroy(&Q);
    return TEST_OK;
}

int main(void)
{
    srand(1);

    test_suite_start();
    run_test(test_priority_queue_init);
    run_test(test_priority_queue_usage);
    run_test(test_priority_queue_clear);
    run_test(test_priority_queue_destroy);
    test_suite_end();
}
