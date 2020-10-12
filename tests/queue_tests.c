#include "debug.h"
#include "queue.h"
#include "unittest.h"

static Queue Q;
static int rc;

int test_queue_init(void)
{
    rc = Queue_init(&Q, sizeof(int), NULL);
    test(rc == 0, "Queue_init failed.");
    test(Q.first == NULL && Q.last == NULL, "Q.first != NULL or Q.last != NULL");
    test(Q.element_size == sizeof(int),
            "Q.element_size = %lu (%lu)", Q.element_size, sizeof(int));
    test(Q.size == 0, "Q.size = %lu (%lu)", Q.size, 0lu);
    test(Q.destroy == NULL, "Q.destroy = %p (%p)", Q.destroy, NULL);
    return TEST_OK;
}

int test_queue_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        rc = Queue_push(&Q, &i);
        test(rc == 0, "Queue_push failed (loop iteration i=%d)", i);
    }
    test(Q.size == 8, "Q.size = %lu (%lu)", Q.size, 8lu);

    for (int i = 0; i < 8; ++i) {
        rc = Queue_pop(&Q, &val);
        test(rc == 0, "Queue_pop failed (loop iteration i=%d)", i);
        test(val == i, "val = %d (%d) (loop iteration i=%d)", val, i, i);
    }
    test(Q.size == 0, "Q.size = %lu (%lu)", Q.size, 0lu);

    return TEST_OK;
}

int test_queue_clear(void)
{

    Queue_clear(&Q);
    test(Q.size == 0, "Q.size = %lu (%lu)", Q.size, 0lu);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_queue_init);
    run_test(test_queue_usage);
    run_test(test_queue_clear);
    test_suite_end();
}
