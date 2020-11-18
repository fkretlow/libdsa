#include "log.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"
#include "vector.h"

static Vector *V;
static int rc;

int test_vector_new(void)
{
    V = Vector_new(&int_type);
    test(V->data != NULL);
    test(V->size == 0);
    test(V->capacity == VECTOR_MIN_CAPACITY);
    test(V->element_type->size == sizeof(int));
    return 0;
}

int test_vector_usage(void)
{
    int in = 1;
    int out = 0;

    rc = Vector_push_back(V, &in);
    test(rc == 0);
    test(V->size == 1);

    rc = Vector_get(V, 0, &out);
    test(rc == 0);
    test(out == in);

    out = 0;
    rc = Vector_pop_back(V, &out);
    test(rc == 0);
    test(V->size == 0);
    test(out == in);

    for (int i = 0; i < 17; ++i) {
        rc = Vector_push_back(V, &i);
        test(rc == 0);
        test(V->size == (size_t)i + 1);
    }
    test(V->capacity == 32);

    for (int i = 16; i >= 0; --i) {
        rc = Vector_pop_back(V, &out);
        test(rc == 0);
        test(out == i);
    }
    test(V->capacity == VECTOR_MIN_CAPACITY);

    test_fail(Vector_pop_back(V, &out) == -1, "Vector_pop_back should fail.");

    return 0;
}

int test_vector_teardown(void)
{
    Vector_clear(V);
    test(V->data != NULL);
    test(V->capacity == VECTOR_MIN_CAPACITY);
    test(V->size == 0);
    Vector_delete(V);
    return 0;
}

int test_vector_of_strings(void)
{
    V = Vector_new(&str_type);
    test(V != NULL);

    str *s = str_new();
    str_assign_cstr(s, "Haydn");
    Vector_push_back(V, s);

    str_assign_cstr(s, "Mozart");
    Vector_push_back(V, s);

    str_assign_cstr(s, "Beethoven");
    Vector_push_back(V, s);

    str out;
    Vector_pop_back(V, &out);
    test(str_compare(s, &out) == 0);
    str_destroy(&out);

    Vector_delete(V);
    str_delete(s);
    return 0;
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
