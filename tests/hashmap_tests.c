#include "debug.h"
#include "hash.h"
#include "hashmap.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static Hashmap *M;
static int rc, key, value;

int test_hashmap_new(void)
{
    M = Hashmap_new(&int_type, &int_type);
    test(M != NULL,"M = NULL");
    test(M->key_type == &int_type, "M->key_type != &int_type");
    test(M->value_type == &int_type, "M->value_type != &int_type");
    test(M->buckets != NULL, "M->buckets = NULL");

    return TEST_OK;
}

int test_hashmap_usage(void)
{
    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Hashmap_set(M, &i, &j);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Hashmap_get(M, &i, &value);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        test(value == j, "v = %d (%d)", value, j);
    }

    key = 10;
    value = 0;

    rc = Hashmap_has(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    rc = Hashmap_get(M, &key, &value);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(value == 0, "value = %d (%d)", value, 0);

    rc = Hashmap_remove(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    key = 1;

    rc = Hashmap_has(M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Hashmap_get(M, &key, &value);
    test(rc == 1, "rc = %d (%d)", rc, 1);
    test(value == 10, "value = %d (%d)", value, 10);

    rc = Hashmap_remove(M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Hashmap_has(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    return TEST_OK;
}

int test_hashmap_teardown(void)
{
    Hashmap_delete(M);
    return TEST_OK;
}

int test_hashmap_with_strings(void)
{
    M = Hashmap_new(&String_type, &String_type);
    test(M != NULL, "M = NULL");

    String *k = String_from_cstr("name");
    String *v = String_from_cstr("Johann");
    String v_out;
    rc = Hashmap_set(M, k, v);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    rc = Hashmap_get(M, k, &v_out);
    test(rc == 1, "rc = %d (%d)", rc, 1);
    test(String_compare(v, &v_out) == 0, "v != v_out");

    String_delete(k);
    String_delete(v);
    String_destroy(&v_out);
    Hashmap_delete(M);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_hashmap_new);
    run_test(test_hashmap_usage);
    run_test(test_hashmap_teardown);
    run_test(test_hashmap_with_strings);
    test_suite_end();
}
