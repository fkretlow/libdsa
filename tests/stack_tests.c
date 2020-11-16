#include "log.h"
#include "stack.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static Stack *S;
int rc;

int test_stack_new(void)
{
    S = Stack_new(&int_type);
    test(S != NULL);
    test(S->top == NULL);
    test(S->element_type->size == sizeof(int));
    test(S->size == 0);
    return TEST_OK;
}

int test_stack_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        test(!Stack_push(S, &i));
    }
    test(S->size == 8);

    test(*(int*)Stack_top(S) == 7);

    for (int i = 7; i >= 0; --i) {
        test(!Stack_pop(S, &val));
        test(val == i);
    }
    test(S->size == 0);

    test(Stack_top(S) == NULL);
    test_fail(Stack_pop(S, &val) == -1, "Stack_pop from empty stack should fail.");

    return TEST_OK;
}

int test_stack_teardown(void)
{
    Stack_clear(S);
    test(S->size == 0);
    Stack_delete(S);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_stack_new);
    run_test(test_stack_usage);
    run_test(test_stack_teardown);
    test_suite_end();
}
