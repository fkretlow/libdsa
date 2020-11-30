#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bst.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

#define NMEMB 256
#define MAXV 1024

int print_balance(bstn *n, void *p)
{
    p = (void*)p;
    printf("k=%d, b=%d\n", *(int*)((char*)n + sizeof(bstn)), n->flags.avl.balance);
    return 0;
}

int test_avl_insert(void)
{
    bst *T = bst_new(AVL, &int_type, NULL);

    int rc, i, v;
    int values[NMEMB] = { 0 };
    uint32_t count = 0;

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_insert(T, &i);
        ++count;
        test(rc == 1);
        test(bst_count(T) == count);
        test(bst_has(T, &i) == 1);
    }

    v = 0;
    rc = bst_insert(T, &v);
    test(rc == 0);

    bst_clear(T);
    count = 0;

    for (i = NMEMB; i > 0; --i) {
        rc = bst_insert(T, &i);
        ++count;
        test(rc == 1);
    }

    bst_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bst_insert(T, &v);
        test(rc >= 0);
        if (rc == 1) {
            values[i] = v;
            ++count;
        } else {
            --i;
        }
        test(bst_count(T) == count);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_has(T, values + i);
        test(rc == 1);
    }

    bst_delete(T);
    return 0;
}

int test_avl_remove(void)
{
    bst *T = bst_new(AVL, &int_type, NULL);

    int rc, i, v;
    int values[NMEMB] = { 0 };
    uint32_t count = 0;

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_insert(T, &i);
        ++count;
        test(rc >= 0);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_remove(T, &i);
        --count;
        test(rc >= 0);
        test(bst_count(T) == count);
    }

    test(T->root == NULL);

    /*from here on it's just testing insert...*/
    v = 0;
    rc = bst_remove(T, &v);
    test(rc == 0);

    bst_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bst_insert(T, &v);
        test(rc >= 0);
        if (rc == 1) {
            values[i] = v;
            ++count;
        } else {
            --i;
        }
        test(bst_count(T) == count);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_remove(T, values + i);
        test(rc == 1);
    }

    test(T->root == NULL);

    bst_delete(T);
    return 0;
}

int test_avl_set_get(void)
{
    bst *T = bst_new(AVL, &str_type, &int_type);
    test(T);

    int rc, i, *v;
    str *s;
    str *keys[NMEMB] = { 0 };
    int values[NMEMB] = { 0 };

    for (i = 0; i < NMEMB; ++i) {
        s = random_str(8);
        while (bst_has(T, s)) {
            str_delete(s);
            s = random_str(8);
        }
        keys[i] = s;
        values[i] = i;
        rc = bst_set(T, s, &i);
        test(rc >= 0);
    }

    for (i = 0; i < NMEMB; ++i) {
        v = bst_get(T, keys[i]);
        test(v != NULL);
        test(*v == values[i]);
    }

    for (i = 0; i < NMEMB; ++i) {
        values[i] *= 10;
        rc = bst_set(T, keys[i], values + i);
        test(rc == 0);
    }

    for (i = 0; i < NMEMB; ++i) {
        v = bst_get(T, keys[i]);
        test(v != NULL);
        test(*v == values[i]);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_remove(T, keys[i]);
        test(rc == 1);
        rc = bst_has(T, keys[i]);
        test(rc == 0);
    }

    for (i = 0; i < NMEMB; ++i) str_delete(keys[i]);
    bst_delete(T);
    return 0;
}

int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* log_info("random seed was %u", seed); */

    run_test(test_avl_insert);
    run_test(test_avl_remove);
    run_test(test_avl_set_get);

    test_suite_end();
}
