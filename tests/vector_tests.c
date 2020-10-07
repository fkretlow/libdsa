#include <stdio.h>

#include "debug.h"
#include "unittest.h"
#include "vector.h"

static Vector V;
static int rc;

int test_vector_init(void)
{
    rc = Vector_init(&V, sizeof(int), NULL);
    test(V.data != NULL, "V.data = NULL");
    test(V.end == 0, "V.end = %lu (%lu)", V.end, 0lu);
    test(V.max == VECTOR_MIN_CAPACITY,
            "V.max = %lu (%lu)", V.max, VECTOR_MIN_CAPACITY);
    test(V.element_size == sizeof(int),
            "V.element_size = %lu (%lu)", V.element_size, sizeof(int));
    test(V.destroy == NULL, "V.destroy != NULL");
    return TEST_OK;
}

int test_vector_usage(void)
{
    int in = 1;
    int out = 0;
    rc = Vector_push_back(&V, &in);
    test(rc == 0, "Vector_push failed.");
    test(V.end == 1, "V.end = %lu (%lu)", V.end, 1lu);
    rc = Vector_get(&V, 0, &out);
    test(rc == 0, "Vector_get failed.");
    test(out == in, "out = %d (%d)", out, in);

    for (int i = 0; i < 17; ++i) {
        rc = Vector_push_back(&V, &i);
        test(rc == 0, "Vector_push failed (loop i = %d)", i);
        test(V.end == 2 + i, "V.end = %lu (%lu)", V.end, 2lu + (unsigned long)i);
    }
    test(V.max == 32, "V.max = %lu (%lu)", V.max, 32lu);

    return TEST_OK;
}

int test_vector_clear(void)
{
    Vector_clear(&V);
    test(V.data != NULL, "V.data = NULL");
    test(V.max == VECTOR_MIN_CAPACITY,
            "V.max = %lu (%lu)", V.max, VECTOR_MIN_CAPACITY);
    test(V.end == 0, "V.end = %lu (%lu)", V.end, 0lu);
    return TEST_OK;
}

int test_vector_destroy(void)
{
    Vector_destroy(&V);
    test(V.data == NULL, "V.data != NULL");
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_vector_init);
    run_test(test_vector_usage);
    run_test(test_vector_clear);
    run_test(test_vector_destroy);
    test_suite_end();
}
