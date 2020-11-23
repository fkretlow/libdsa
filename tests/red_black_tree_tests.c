#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "binary_tree.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

#define NMEMB 256
#define MAXV 1024

int test_rbt_copy(void)
{
    bt *T = bt_new(RED_BLACK, &str_type, &int_type);

    /* set up a source tree */
    str *kn  = str_from_cstr("1 root");
    str *kl  = str_from_cstr("0 left");
    str *kr  = str_from_cstr("3 right");
    str *krl = str_from_cstr("2 right-left");
    int vn  = 0;
    int vl  = 1;
    int vr  = 2;
    int vrl = 3;

    btn *n = btn_new(T);
    btn_set_key(T, n, kn);
    btn_set_value(T, n, &vn);
    rbtn_set_color(n, BLACK);

    btn *l = btn_new(T);
    btn_set_key(T, l, kl);
    btn_set_value(T, l, &vl);
    rbtn_set_color(l, BLACK);

    btn *r = btn_new(T);
    btn_set_key(T, r, kr);
    btn_set_value(T, r, &vr);
    rbtn_set_color(r, BLACK);

    btn *rl = btn_new(T);
    btn_set_key(T, rl, krl);
    btn_set_value(T, rl, &vrl);
    rbtn_set_color(rl, RED);

    T->root = n;
    T->count = 4;
    n->left = l; l->parent = n;
    n->right = r; r->parent = n;
    r->left = rl; rl->parent = r;

    /* copy and verify */
    bt *C = bt_copy(T);
    test(C->count == T->count);
    test(C->key_type == T->key_type);
    test(C->value_type == T->value_type);

    btn *c = C->root;
    btn *cl = c->left;
    btn *cr = c->right;
    btn *crl = c->right->left;

    test(c);
    test(rbtn_color(c) == BLACK);
    test(str_compare(btn_key  (T, c), kn)  == 0);
    test(int_compare(btn_value(T, c), &vn) == 0);

    test(cl);
    test(rbtn_color(cl) == BLACK);
    test(str_compare(btn_key  (T, cl), kl)  == 0);
    test(int_compare(btn_value(T, cl), &vl) == 0);
    test(cl->left == NULL && cl->right == NULL);

    test(cr);
    test(rbtn_color(cr) == BLACK);
    test(str_compare(btn_key  (T, cr), kr)  == 0);
    test(int_compare(btn_value(T, cr), &vr) == 0);
    test(cr->left != NULL && cr->right == NULL);

    test(crl);
    test(rbtn_color(crl) == RED);
    test(str_compare(btn_key  (T, crl), krl)  == 0);
    test(int_compare(btn_value(T, crl), &vrl) == 0);
    test(crl->left == NULL && crl->right == NULL);

    /* also do it once on the stack */
    bt S;
    int rc = bt_copy_to(&S, T);
    test(rc == 0);
    test(S.count == T->count);
    test(S.key_type == T->key_type);
    test(S.value_type == T->value_type);

    /* teardown */
    bt_delete(T);
    bt_delete(C);
    bt_destroy(&S);

    str_delete(kn);
    str_delete(kl);
    str_delete(kr);
    str_delete(krl);
    return 0;
}

int test_rbt_insert(void)
{
    bt *T = bt_new(RED_BLACK, &int_type, NULL);

    int rc, i, v;
    int values[NMEMB] = { 0 };
    uint32_t count = 0;

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_insert(T, &i);
        ++count;
        test(rc == 1);
        test(bt_count(T) == count);
        test(bt_has(T, &i) == 1);
    }

    v = 0;
    rc = bt_insert(T, &v);
    test(rc == 0);

    bt_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bt_insert(T, &v);
        test(rc >= 0);
        if (rc == 1) {
            ++count;
            values[i] = v;
        } else {
            --i;
        }
        test(bt_count(T) == count);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_has(T, values + i);
        test(rc == 1);
    }

    bt_delete(T);
    return 0;
}

int test_rbt_remove(void)
{
    bt *T = bt_new(RED_BLACK, &int_type, NULL);

    int rc, i, v;
    int values[NMEMB] = { 0 };
    uint32_t count = 0;

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_insert(T, &i);
        ++count;
        test(rc == 1);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_remove(T, &i);
        --count;
        test(rc == 1);
        test(bt_count(T) == count);
    }

    test(T->root == NULL);

    /*from here on it's just testing insert...*/
    v = 0;
    rc = bt_remove(T, &v);
    test(rc == 0);

    bt_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bt_insert(T, &v);
        test(rc >= 0);
        if (rc == 1) {
            ++count;
            values[i] = v;
        } else {
            --i;
        }
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_remove(T, values + i);
        --count;
        test(rc == 1);
        test(bt_count(T) == count);
    }

    test(T->root == NULL);

    bt_delete(T);
    return 0;
}

int test_rbt_set_get(void)
{
    bt *T = bt_new(RED_BLACK, &str_type, &int_type);
    test(T);

    int rc, i, *v;
    str *s;
    str *keys[NMEMB] = { 0 };
    int values[NMEMB] = { 0 };

    for (i = 0; i < NMEMB; ++i) {
        s = random_str(8);
        while (bt_has(T, s)) {
            str_delete(s);
            s = random_str(8);
        }
        keys[i] = s;
        values[i] = i;
        rc = bt_set(T, s, &i);
        test(rc == 1);
    }

    for (i = 0; i < NMEMB; ++i) {
        v = bt_get(T, keys[i]);
        test(v != NULL);
        test(*v == values[i]);
    }

    for (i = 0; i < NMEMB; ++i) {
        values[i] *= 10;
        rc = bt_set(T, keys[i], &values[i]);
        test(rc == 0);
    }

    for (i = 0; i < NMEMB; ++i) {
        v = bt_get(T, keys[i]);
        test(v != NULL);
        test(*v == values[i]);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bt_remove(T, keys[i]);
        test(rc == 1);
        rc = bt_has(T, keys[i]);
        test(rc == 0);
    }

    for (i = 0; i < NMEMB; ++i) str_delete(keys[i]);
    bt_delete(T);
    return 0;
}

int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* log_info("random seed was %u", seed); */

    run_test(test_rbt_copy);
    run_test(test_rbt_insert);
    run_test(test_rbt_remove);
    run_test(test_rbt_set_get);

    test_suite_end();
}
