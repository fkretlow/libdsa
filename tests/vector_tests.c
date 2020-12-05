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
    int rc, in, out;
    in = 1;

    rc = vector_push_back(V, &in);
    test(rc == 1);
    test(V->count == 1);

    test(*(int*)vector_get(V, 0) == in);

    rc = vector_pop_back(V, &out);
    test(rc == 1);
    test(out == in);
    test(vector_count(V) == 0);

    for (int i = 0; i < 17; ++i) {
        rc = vector_push_back(V, &i);
        test(rc == 1);
        test(vector_count(V) == (size_t)i + 1);
    }
    test(V->capacity == 32);

    int *v = vector_get(V, 0);
    test(*v == 0);
    v = vector_get(V, 16);
    test(*v == 16);

    in = -1;
    rc = vector_insert(V, 0, &in);
    test(rc == 1);
    v = vector_get(V, 16);
    test(*v == 15);

    rc = vector_remove(V, 1);
    test(rc == 1);
    v = vector_get(V, 0);
    test(*v == -1);
    v = vector_get(V, 1);
    test(*v == 1);

    in = 0;
    rc = vector_set(V, 0, &in);
    test(rc == 0);
    v = vector_get(V, 0);
    test(*v == 0);

    for (int i = 16; i >= 0; --i) {
        rc = vector_pop_back(V, &out);
        test(rc == 1);
        test(out == i);
    }
    test(V->capacity == VECTOR_MIN_CAPACITY);

    rc = vector_pop_back(V, NULL);
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

    str out;
    str *s = str_new();
    str *sp;

    str_assign_cstr(s, "Haydn");
    vector_push_back(V, s);

    str_assign_cstr(s, "Mozart");
    vector_push_back(V, s);

    str_assign_cstr(s, "Beethoven");
    vector_push_back(V, s);

    str *last = vector_last(V);
    test(str_compare(s, last) == 0);

    vector_pop_back(V, &out);
    test(str_compare(s, &out) == 0);

    str_assign_cstr(s, "Bach");
    vector_insert(V, 1, s);

    sp = vector_get(V, 1);
    test(str_compare(sp, s) == 0);

    vector_remove(V, 1);
    sp = vector_get(V, 1);
    test(strncmp(str_data(sp), "Mozart", 16) == 0);

    vector_delete(V);
    str_delete(s);
    str_destroy(&out);
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
