#include "debug.h"
#include "hash.h"
#include "map.h"
#include "str.h"
#include "test_utils.h"
#include "test.h"

static Map M;
static int rc, key, value;

int test_map_new(void)
{
    M = Map_new(sizeof(int), sizeof(int),
                jenkins_hash, compint,
                NULL, NULL, NULL, NULL);
    test(M != NULL,"M = NULL");
    test (M->key_size == sizeof(int),
            "M->key_size = %lu (%lu)", M->key_size, sizeof(int));
    test (M->value_size == sizeof(int),
            "M->value_size = %lu (%lu)", M->value_size, sizeof(int));
    test(M->buckets != NULL, "M->buckets = NULL");

    return TEST_OK;
}

int test_map_usage(void)
{
    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Map_set(M, &i, &j);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Map_get(M, &i, &value);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        test(value == j, "v = %d (%d)", value, j);
    }

    key = 10;
    value = 0;

    rc = Map_has(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    rc = Map_get(M, &key, &value);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(value == 0, "value = %d (%d)", value, 0);

    rc = Map_remove(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    key = 1;

    rc = Map_has(M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Map_get(M, &key, &value);
    test(rc == 1, "rc = %d (%d)", rc, 1);
    test(value == 10, "value = %d (%d)", value, 10);

    rc = Map_remove(M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Map_has(M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    return TEST_OK;
}

int test_map_teardown(void)
{
    Map_delete(M);
    return TEST_OK;
}

int test_map_with_strings(void)
{
    M = Map_new(sizeof(String), sizeof(String),
                String_hash, String_compare,
                String_copy_to, String_delete,
                String_copy_to, String_delete);
    test(M != NULL, "M = NULL");

    String k = String_from_cstr("name");
    String v = String_from_cstr("Johann");
    String v_out = NULL;
    rc = Map_set(M, &k, &v);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(!Map_get(M, &k, &v_out), "Failed to get value mapped to string.");
    test(v_out != NULL, "v_out = NULL");
    test(String_compare(v, v_out) == 0, "v != v_out");

    String_delete(k);
    String_delete(v);
    String_delete(v_out);
    Map_delete(M);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_map_new);
    run_test(test_map_usage);
    run_test(test_map_teardown);
    run_test(test_map_with_strings);
    test_suite_end();
}
