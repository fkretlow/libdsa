#include "log.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"
#include "vector.h"

static vector *V;

int test_vector_new(void)
{
    V = vector_new(&int_type);
    test(V->data != NULL);
    test(V->count == 0);
    test(V->capacity == VECTOR_MIN_CAPACITY);
    test(V->data_type->size == sizeof(int));
    return 0;
}

int test_vector_usage(void)
{
    int rc, in, *out;
    in = 1;

    rc = vector_push_back(V, &in);
    test(rc == 0);
    test(V->count == 1);

    out = vector_get(V, 0);
    test(*out == in);

    rc = vector_pop_back(V);
    test(rc == 1);
    test(V->count == 0);

    for (int i = 0; i < 17; ++i) {
        rc = vector_push_back(V, &i);
        test(rc == 0);
        test(V->count == (size_t)i + 1);
    }
    test(V->capacity == 32);

    for (int i = 16; i >= 0; --i) {
        rc = vector_pop_back(V);
        test(rc == 1);
    }
    test(V->capacity == VECTOR_MIN_CAPACITY);

    rc = vector_pop_back(V);
    test(rc == 0);

    return 0;
}

int test_vector_teardown(void)
{
    vector_clear(V);
    test(V->data != NULL);
    test(V->capacity == VECTOR_MIN_CAPACITY);
    test(V->count == 0);
    vector_delete(V);
    return 0;
}

int test_vector_of_strings(void)
{
    V = vector_new(&str_type);
    test(V != NULL);

    str *s = str_new();
    str_assign_cstr(s, "Haydn");
    vector_push_back(V, s);

    str_assign_cstr(s, "Mozart");
    vector_push_back(V, s);

    str_assign_cstr(s, "Beethoven");
    vector_push_back(V, s);

    str *last = vector_last(V);
    test(str_compare(s, last) == 0);

    vector_pop_back(V);

    vector_delete(V);
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
