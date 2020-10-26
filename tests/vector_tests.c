#include "debug.h"
#include "str.h"
#include "test.h"
#include "vector.h"

static Vector V;
static int rc;

int test_vector_new(void)
{
    V = Vector_new(sizeof(int), NULL, NULL);
    test(V->data != NULL, "V->data = NULL");
    test(V->size == 0, "V->size = %lu (%lu)", V->size, 0lu);
    test(V->capacity == VECTOR_MIN_CAPACITY,
            "V->capacity = %lu (%lu)", V->capacity, VECTOR_MIN_CAPACITY);
    test(V->element_size == sizeof(int),
            "V->element_size = %lu (%lu)", V->element_size, sizeof(int));
    test(V->destroy_element == NULL, "V->destroy_element != NULL");
    return TEST_OK;
}

int test_vector_usage(void)
{
    int in = 1;
    int out = 0;

    rc = Vector_push_back(V, &in);
    test(rc == 0, "Vector_push_back failed.");
    test(V->size == 1, "V->size = %lu (%lu)", V->size, 1lu);

    rc = Vector_get(V, 0, &out);
    test(rc == 0, "Vector_get failed.");
    test(out == in, "out = %d (%d)", out, in);

    out = 0;
    rc = Vector_pop_back(V, &out);
    test(rc == 0, "Vector_pop_back failed.");
    test(V->size == 0, "V->size = %lu (%lu)", V->size, 0lu);
    test(out == in, "out = %d (%d)", out, in);

    for (int i = 0; i < 17; ++i) {
        rc = Vector_push_back(V, &i);
        test(rc == 0, "Vector_push_back failed (loop i = %d)", i);
        test(V->size == (size_t)i + 1,
                "V->size = %lu (%lu)", V->size, (unsigned long)i + 1);
    }
    test(V->capacity == 32,
            "V->capacity = %lu (%lu) after 17 * push_back", V->capacity, 32lu);

    for (int i = 16; i >= 0; --i) {
        rc = Vector_pop_back(V, &out);
        test(rc == 0, "Vector_pop_back failed (loop i = %d (decrementing))", i);
        test(out == i, "out = %d (%d) (loop i = %d (decrementing))", out, i, i);
    }
    test(V->capacity == VECTOR_MIN_CAPACITY,
            "V->capacity = %lu (%lu) after removing all elements",
            V->capacity, VECTOR_MIN_CAPACITY);

    test_fail(Vector_pop_back(V, &out) == -1, "Vector_pop_back should fail.");

    return TEST_OK;
}

int test_vector_teardown(void)
{
    Vector_clear(V);
    test(V->data != NULL, "V->data = NULL");
    test(V->capacity == VECTOR_MIN_CAPACITY,
            "V->capacity = %lu (%lu)", V->capacity, VECTOR_MIN_CAPACITY);
    test(V->size == 0, "V->size = %lu (%lu)", V->size, 0lu);
    Vector_delete(V);
    return TEST_OK;
}

int test_vector_of_strings(void)
{
    V = Vector_new(sizeof(String), String_copy_to, String_delete);
    test(V != NULL, "V = NULL");

    String s = String_new();
    String_assign_cstr(s, "Haydn");
    Vector_push_back(V, &s);

    String_assign_cstr(s, "Mozart");
    Vector_push_back(V, &s);

    String_assign_cstr(s, "Beethoven");
    Vector_push_back(V, &s);

    String out;
    Vector_pop_back(V, &out);
    test(String_compare(s, out) == 0, "s != out (out = \"%s\")", out->data);
    String_delete(out);

    Vector_delete(V);
    String_delete(s);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_vector_new);
    run_test(test_vector_usage);
    run_test(test_vector_teardown);
    run_test(test_vector_of_strings);
    test_suite_end();
}
