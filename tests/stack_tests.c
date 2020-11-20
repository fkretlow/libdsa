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
    test(S->top == NULL);
    test(S->data_type->size == sizeof(int));
    test(S->count == 0);
    return 0;
}

int test_stack_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        test(!stack_push(S, &i));
    }
    test(S->count == 8);

    test(*(int*)stack_top(S) == 7);

    for (int i = 7; i >= 0; --i) {
        test(!stack_pop(S, &val));
        test(val == i);
    }
    test(S->count == 0);

    test(stack_top(S) == NULL);
    test_fail(stack_pop(S, &val) == -1, "stack_pop from empty stack should fail.");

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
