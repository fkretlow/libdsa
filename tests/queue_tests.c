#include "debug.h"
#include "queue.h"
#include "test.h"

static Queue Q;
static int rc;

int test_queue_new(void)
{
    Q = Queue_new(sizeof(int), NULL, NULL);
    test(Q != NULL, "Q = NULL");
    test(Q->first == NULL && Q->last == NULL, "Q->first != NULL or Q->last != NULL");
    test(Q->element_size == sizeof(int),
            "Q->element_size = %lu (%lu)", Q->element_size, sizeof(int));
    test(Q->size == 0, "Q->size = %lu (%lu)", Q->size, 0lu);
    test(Q->destroy_element == NULL,
            "Q->destroy_element = %p (%p)", Q->destroy_element, NULL);
    return TEST_OK;
}

int test_queue_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        rc = Queue_enqueue(Q, &i);
        test(rc == 0, "Queue_enqueue failed (loop iteration i=%d)", i);
    }
    test(Q->size == 8, "Q->size = %lu (%lu)", Q->size, 8lu);

    for (int i = 0; i < 8; ++i) {
        rc = Queue_dequeue(Q, &val);
        test(rc == 0, "Queue_dequeue failed (loop iteration i=%d)", i);
        test(val == i, "val = %d (%d) (loop iteration i=%d)", val, i, i);
    }
    test(Q->size == 0, "Q->size = %lu (%lu)", Q->size, 0lu);

    return TEST_OK;
}

int test_queue_clear_delete(void)
{

    Queue_clear(Q);
    test(Q->size == 0, "Q->size = %lu (%lu)", Q->size, 0lu);
    Queue_delete(Q);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_queue_new);
    run_test(test_queue_usage);
    run_test(test_queue_clear_delete);
    test_suite_end();
}
