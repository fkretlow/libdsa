#include "debug.h"
#include "hash.h"
#include "map.h"
#include "str.h"
#include "test_utils.h"
#include "test.h"

static Map M;
static int rc, key, value;

int test_map_init(void)
{
    rc = Map_init(&M, sizeof(int), sizeof(int),
                  jenkins_hash, compint,
                  NULL, NULL);
    test(rc == 0,"Map_init failed.");
    test (M.key_size == sizeof(int),
            "M.key_size = %lu (%lu)", M.key_size, sizeof(int));
    test (M.value_size == sizeof(int),
            "M.value_size = %lu (%lu)", M.value_size, sizeof(int));
    test(M.buckets != NULL, "M.buckets = NULL");

    return TEST_OK;
}

int test_map_usage(void)
{
    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Map_set(&M, &i, &j);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = Map_get(&M, &i, &value);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        test(value == j, "v = %d (%d)", value, j);
    }

    key = 10;
    value = 0;

    rc = Map_has(&M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    rc = Map_get(&M, &key, &value);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(value == 0, "value = %d (%d)", value, 0);

    rc = Map_delete(&M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    key = 1;

    rc = Map_has(&M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Map_get(&M, &key, &value);
    test(rc == 1, "rc = %d (%d)", rc, 1);
    test(value == 10, "value = %d (%d)", value, 10);

    rc = Map_delete(&M, &key);
    test(rc == 1, "rc = %d (%d)", rc, 1);

    rc = Map_has(&M, &key);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    return TEST_OK;
}

int test_map_clear(void)
{
    Map_clear(&M);
    return TEST_OK;
}

int test_map_destroy(void)
{
    Map_destroy(&M);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_map_init);
    run_test(test_map_usage);
    run_test(test_map_clear);
    run_test(test_map_destroy);
    test_suite_end();
}
