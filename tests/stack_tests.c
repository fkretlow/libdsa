#include "log.h"
#include "stack.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static stack *S;
int rc;

int test_stack_new(void)
{
    S = stack_new(&int_type);
    test(S != NULL);
    test(stack_top(S) == NULL);
    test(S->data_type->size == sizeof(int));
    test(stack_count(S) == 0);
    return 0;
}

int test_stack_usage(void)
{
    int val;
    int *ip;
    int rc;

    for (int i = 0; i < 8; ++i) {
        rc = stack_push(S, &i);
        test(rc == 1);
    }
    test(stack_count(S) == 8);

    ip = stack_top(S);
    test(*ip == 7);

    for (int i = 7; i >= 0; --i) {
        rc = stack_pop(S, &val);
        test(rc == 1);
        test(val == i);
    }
    test(stack_count(S) == 0);

    test(stack_top(S) == NULL);

    rc = stack_pop(S, &val);
    test(rc == 0);

    return 0;
}

int test_stack_teardown(void)
{
    stack_clear(S);
    test(S->count == 0);
    stack_delete(S);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_stack_new);
    run_test(test_stack_usage);
    run_test(test_stack_teardown);
    test_suite_end();
}
