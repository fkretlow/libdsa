#include "log.h"
#include "hash.h"
#include "hashmap.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static hashmap *M;
static int rc, k, v;

int test_hashmap_new(void)
{
    M = hashmap_new(&int_type, &int_type);
    test(M != NULL);
    test(M->key_type == &int_type);
    test(M->value_type == &int_type);
    test(M->buckets != NULL);

    return 0;
}

int test_hashmap_usage(void)
{
    int *vp;

    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        rc = hashmap_set(M, &i, &j);
        test(rc == 1);
        test(hashmap_count(M) == (size_t)i + 1);
    }

    for (int i = 0, j = 0; i < 10; ++i, j = 10 * i) {
        vp = hashmap_get(M, &i);
        test(vp);
        test(*vp == j);
    }

    k = 10;
    v = 0;

    rc = hashmap_has(M, &k);
    test(rc == 0);

    vp = hashmap_get(M, &k);
    test(!vp);

    rc = hashmap_remove(M, &k);
    test(rc == 0);

    k = 1;

    rc = hashmap_has(M, &k);
    test(rc == 1);

    vp = hashmap_get(M, &k);
    test(vp);
    test(*vp == 10);

    rc = hashmap_remove(M, &k);
    test(rc == 1);
    test(hashmap_count(M) == 9);

    rc = hashmap_has(M, &k);
    test(rc == 0);

    return 0;
}

int test_hashmap_teardown(void)
{
    hashmap_delete(M);
    return 0;
}

int test_hashmap_with_strings(void)
{
    M = hashmap_new(&str_type, &str_type);
    test(M);

    str *k = str_from_cstr("name");
    str *v = str_from_cstr("Johann");
    str *r;
    rc = hashmap_set(M, k, v);
    test(rc == 1);
    r = hashmap_get(M, k);
    test(r);
    test(str_compare(v, r) == 0);

    str_delete(k);
    str_delete(v);
    hashmap_delete(M);
    return 0;
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
