#include <time.h>

#include "set.h"
#include "test.h"
#include "type_interface.h"

#define NMEMB 100

static set *S;
static int rc;

/* static int print_node(_rbt_node *n, void *nothing)
{
    nothing = nothing;
    printf("%d %s\n", *(int*)n->data, n->color == RED ? "R" : "B");
    return 0;
} */

int test_set_init(void)
{
    S = set_new(&int_type);
    return 0;
}

int test_set_usage(void)
{
    for (int i = 0; i < NMEMB; ++i) {
        rc = set_insert(S, &i);
        test(rc == 1);
    }

    for (int i = 0; i < NMEMB; ++i) {
        rc = set_has(S, &i);
        test(rc == 1);
    }

    int v = -1;
    rc = set_has(S, &v);
    test(rc == 0);

    for (int i = 0; i < NMEMB; ++i) {
        rc = set_remove(S, &i);
        test(rc == 1);
    }

    return 0;
}

int test_set_teardown(void)
{
    set_clear(S);
    test(S->count == 0);
    test(S->root == NULL);
    set_delete(S);
    return 0;
}

int test_set_union(void)
{
    set *S1 = set_new(&int_type);
    set *S2 = set_new(&int_type);

    for (int i = 0; i < 10; ++i) {
        set_insert(S1, &i);
    }
    for (int i = 4; i < 15; ++i) {
        set_insert(S2, &i);
    }

    set *U = set_union(S1, S2);
    test(U != NULL);

    for (int i = 0; i < 15; ++i) {
        rc = set_has(U, &i);
        test(rc == 1);
    }

    set_delete(S1);
    set_delete(S2);
    set_delete(U);
    return 0;
}

int test_set_intersection(void)
{
    set *S1 = set_new(&int_type);
    set *S2 = set_new(&int_type);

    for (int i = 0; i < 10; ++i) {
        set_insert(S1, &i);
    }

    for (int i = 4; i < 15; ++i) {
        set_insert(S2, &i);
    }

    set *I = set_intersection(S1, S2);
    test(I != NULL);

    for (int i = 0; i < 4; ++i) {
        rc = set_has(I, &i);
        test(rc == 0);
    }
    for (int i = 4; i < 10; ++i) {
        rc = set_has(I, &i);
        test(rc == 1);
    }
    for (int i = 10; i < 15; ++i) {
        rc = set_has(I, &i);
        test(rc == 0);
    }

    set_delete(S1);
    set_delete(S2);
    set_delete(I);
    return 0;
}

int test_set_difference(void)
{
    set *S1 = set_new(&int_type);
    set *S2 = set_new(&int_type);

    for (int i = 0; i < 10; ++i) {
        set_insert(S1, &i);
    }

    for (int i = 4; i < 15; ++i) {
        set_insert(S2, &i);
    }

    set *D = set_difference(S1, S2);

    for (int i = 0; i < 4; ++i) {
        rc = set_has(D, &i);
        test(rc == 1);
    }
    for (int i = 4; i < 15; ++i) {
        rc = set_has(D, &i);
        test(rc == 0);
    }

    set_delete(S1);
    set_delete(S2);
    set_delete(D);
    return 0;
}

int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* log_info("random seed was %u", seed); */

    run_test(test_set_init);
    run_test(test_set_usage);
    run_test(test_set_teardown);
    run_test(test_set_union);
    run_test(test_set_intersection);
    run_test(test_set_difference);

    test_suite_end();
}
