#include "debug.h"
#include "stack.h"
#include "unittest.h"

static Stack S;
int rc;

int test_stack_init(void)
{
    rc = Stack_init(&S, sizeof(int), NULL);
    test(rc == 0, "Stack_init failed.");
    test(S.top == NULL, "S.top != NULL");
    test(S.element_size == sizeof(int),
            "S.element_size = %lu (%lu)", S.element_size, sizeof(int));
    test(S.size == 0, "S.size = %lu (%lu)", S.size, 0lu);
    test(S.destroy == NULL, "S.destroy = %p (%p)", S.destroy, NULL);
    return TEST_OK;
}

int test_stack_usage(void)
{
    int val;

    for (int i = 0; i < 8; ++i) {
        test(!Stack_push(&S, &i), "Stack_push failed (for loop i=%d)", i);
    }
    test(S.size == 8, "S.size = %lu (%lu)", S.size, 8lu);

    for (int i = 7; i >= 0; --i) {
        test(!Stack_pop(&S, &val), "Stack_pop failed (for loop i=%d)", i);
        test(val == i, "val = %d (%d)", val, i);
    }
    test(S.size == 0, "S.size = %lu (%lu)", S.size, 0lu);

    test_fail(Stack_pop(&S, &val) == -1, "Stack_pop from empty stack should fail.");

    return TEST_OK;
}

int test_stack_clear(void)
{
    Stack_clear(&S);
    test(S.size == 0, "S.size = %lu (%lu)", S.size, 0lu);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_stack_init);
    run_test(test_stack_usage);
    run_test(test_stack_clear);
    test_suite_end();
}
