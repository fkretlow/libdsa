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

int test_rb_copy(void)
{
    bst *T = bst_new(RB, &str_type, &int_type);

    /* set up a source tree */
    str *kn  = str_from_cstr("1 root");
    str *kl  = str_from_cstr("0 left");
    str *kr  = str_from_cstr("3 right");
    str *krl = str_from_cstr("2 right-left");
    int vn  = 0;
    int vl  = 1;
    int vr  = 2;
    int vrl = 3;

    bst_n *n = bst_n_new(T, kn, &vn);
    n->flags.rb.color = BLACK;

    bst_n *l = bst_n_new(T, kl, &vl);
    l->flags.rb.color = BLACK;

    bst_n *r = bst_n_new(T, kr, &vr);
    r->flags.rb.color = BLACK;

    bst_n *rl = bst_n_new(T, krl, &vrl);

    T->root = n;
    T->count = 4;
    n->left = l;
    n->right = r;
    r->left = rl;

    /* copy and verify */
    bst *C = bst_copy(T);
    test(C->count == T->count);
    test(C->key_type == T->key_type);
    test(C->value_type == T->value_type);

    bst_n *c = C->root;
    bst_n *cl = c->left;
    bst_n *cr = c->right;
    bst_n *crl = c->right->left;

    test(c);
    test(c->flags.rb.color == BLACK);
    test(str_compare(bst_n_key  (T, c), kn)  == 0);
    test(int_compare(bst_n_value(T, c), &vn) == 0);

    test(cl);
    test(cl->flags.rb.color == BLACK);
    test(str_compare(bst_n_key  (T, cl), kl)  == 0);
    test(int_compare(bst_n_value(T, cl), &vl) == 0);
    test(cl->left == NULL && cl->right == NULL);

    test(cr);
    test(cr->flags.rb.color == BLACK);
    test(str_compare(bst_n_key  (T, cr), kr)  == 0);
    test(int_compare(bst_n_value(T, cr), &vr) == 0);
    test(cr->left != NULL && cr->right == NULL);

    test(crl);
    test(crl->flags.rb.color == RED);
    test(str_compare(bst_n_key  (T, crl), krl)  == 0);
    test(int_compare(bst_n_value(T, crl), &vrl) == 0);
    test(crl->left == NULL && crl->right == NULL);

    /* also do it once on the stack */
    bst S;
    int rc = bst_copy_to(&S, T);
    test(rc == 0);
    test(S.count == T->count);
    test(S.key_type == T->key_type);
    test(S.value_type == T->value_type);

    /* teardown */
    bst_delete(T);
    bst_delete(C);
    bst_destroy(&S);

    str_delete(kn);
    str_delete(kl);
    str_delete(kr);
    str_delete(krl);
    return 0;
}

int test_rb_insert(void)
{
    bst *T = bst_new(RB, &int_type, NULL);

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

int test_rb_remove(void)
{
    bst *T = bst_new(RB, &int_type, NULL);

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

int test_rb_set_get(void)
{
    bst *T = bst_new(RB, &str_type, &int_type);
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

    run_test(test_rb_copy);
    run_test(test_rb_insert);
    run_test(test_rb_remove);
    run_test(test_rb_set_get);

    test_suite_end();
}
