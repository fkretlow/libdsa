#include <time.h>

#include "set.h"
#include "test.h"
#include "type_interface.h"

#define MAX_VALUE 1000
#define N_VALUES 100

static Set *S;
static int rc;

/* static int print_node(_rbt_node *n, void *nothing)
{
    nothing = nothing;
    printf("%d %s\n", *(int*)n->data, n->color == RED ? "R" : "B");
    return 0;
} */

int test_set_init(void)
{
    S = Set_new(&int_type);
    return TEST_OK;
}

int test_set_usage(void)
{
    for (int i = 0; i < N_VALUES; ++i) {
        rc = Set_insert(S, &i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    for (int i = 0; i < N_VALUES; ++i) {
        rc = Set_has(S, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    int v = -1;
    rc = Set_has(S, &v);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    for (int i = 0; i < N_VALUES; ++i) {
        rc = Set_remove(S, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    return TEST_OK;
}

int test_set_teardown(void)
{
    Set_clear(S);
    test(S->size == 0, "S->size = %lu (%lu)", S->size, 0lu);
    test(S->root == NULL, "S->root = %p (%p)", S->root, NULL);
    Set_delete(S);
    return TEST_OK;
}

int test_set_union(void)
{
    Set *S1 = Set_new(&int_type);
    Set *S2 = Set_new(&int_type);

    for (int i = 0; i < 10; ++i) {
        Set_insert(S1, &i);
    }
    for (int i = 4; i < 15; ++i) {
        Set_insert(S2, &i);
    }

    Set *U = Set_union(S1, S2);
    test(U != NULL, "Failed to create union of sets.");

    for (int i = 0; i < 15; ++i) {
        rc = Set_has(U, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    Set_delete(S1);
    Set_delete(S2);
    Set_delete(U);
    return TEST_OK;
}

int test_set_intersection(void)
{
    Set *S1 = Set_new(&int_type);
    Set *S2 = Set_new(&int_type);

    for (int i = 0; i < 10; ++i) {
        Set_insert(S1, &i);
    }

    for (int i = 4; i < 15; ++i) {
        Set_insert(S2, &i);
    }

    Set *I = Set_intersection(S1, S2);
    test(I != NULL, "Failed to create intersection of sets.");

    for (int i = 0; i < 4; ++i) {
        rc = Set_has(I, &i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }
    for (int i = 4; i < 10; ++i) {
        rc = Set_has(I, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }
    for (int i = 10; i < 15; ++i) {
        rc = Set_has(I, &i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    Set_delete(S1);
    Set_delete(S2);
    Set_delete(I);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* debug("random seed was %u", seed); */

    run_test(test_set_init);
    run_test(test_set_usage);
    run_test(test_set_teardown);
    run_test(test_set_union);
    run_test(test_set_intersection);

    test_suite_end();
}
