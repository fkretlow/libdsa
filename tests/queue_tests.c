#include "log.h"
#include "queue.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static Queue *Q;
static int rc;

int test_queue_new(void)
{
    Q = Queue_new(&int_type);
    test(Q != NULL);
    test(Q->first == NULL && Q->last == NULL);
    test(Q->data_type->size == sizeof(int));
    test(Q->count == 0);
    return 0;
}

int test_queue_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        rc = Queue_enqueue(Q, &i);
        test(rc == 0);
    }
    test(Q->count == 8);

    for (int i = 0; i < 8; ++i) {
        rc = Queue_dequeue(Q, &val);
        test(rc == 0);
        test(val == i);
    }
    test(Q->count == 0);

    return 0;
}

int test_queue_teardown(void)
{

    Queue_clear(Q);
    test(Q->count == 0);
    Queue_delete(Q);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_queue_new);
    run_test(test_queue_usage);
    run_test(test_queue_teardown);
    test_suite_end();
}
