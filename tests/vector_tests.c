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

int test_vector_clear(void)
{
    Vector_clear(&V);
    test(V.data == NULL, "V.data = NULL");
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
    run_test(test_vector_clear);
    run_test(test_vector_destroy);
    test_suite_end();
}
