#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "binary_tree.h"
#include "str.h"
#include "test.h"
#include "type_interface.h"

#define NMEMB 256
#define MAXV 1024

int test_node_handlers(void)
{
    bt *T = bt_new(NONE, &str_type, &int_type);
    test(T);

    btn *n = btn_new(T);
    test(n);

    str *k1 = str_from_cstr("key");
    str *k2 = str_from_cstr("this is a very long key");

    btn_set_key(T, n, k1);
    test(btn_has_key(n) == 1);
    test(str_compare(btn_key(T, n), k1) == 0);

    btn_destroy_key(T, n);
    test(btn_has_key(n) == 0);

    btn_set_key(T, n, k2);
    test(btn_has_key(n) == 1);
    test(str_compare(btn_key(T, n), k2) == 0);

    int v = 10;
    btn_set_value(T, n, &v);
    test(btn_has_value(n) == 1);
    test(*(int*)btn_value(T, n) == v);

    btn_destroy_value(T, n);
    test(btn_has_value(n) == 0);

    btn_delete(T, n);
    bt_delete(T);
    str_delete(k1);
    str_delete(k2);
    return 0;
}

int test_node_rotations(void)
{
    bt *T = bt_new(NONE, &int_type, NULL);

    btn *p = btn_new(T);
    btn *n = btn_new(T);
    btn *l = btn_new(T);
    btn *ll = btn_new(T);
    btn *lr = btn_new(T);
    btn *r = btn_new(T);
    btn *rl = btn_new(T);
    btn *rr = btn_new(T);

    btn *res;

    test(p);
    test(n);
    test(l);
    test(ll);
    test(lr);
    test(r);
    test(rl);
    test(rr);

    /* construct the test tree */
    p->left = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    /* rotate right and check */
    btn_rotate_right(T, n, &res);
    test(res == l);
    test(p->left == l);
    test(l->parent == p);
    test(l->left == ll);
    test(l->right == n);
    test(n->parent == l);
    test(n->left == lr);
    test(lr->parent == n);

    /* reset the test tree */
    p->left = NULL, p->right = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    /* rotate left and check */
    btn_rotate_left(T, n, &res);
    test(res == r);
    test(p->right == r);
    test(r->parent == p);
    test(r->right == rr);
    test(r->left == n);
    test(n->parent == r);
    test(n->right == rl);
    test(rl->parent == n);

    btn_delete(T, p);
    btn_delete(T, n);
    btn_delete(T, l);
    btn_delete(T, ll);
    btn_delete(T, lr);
    btn_delete(T, r);
    btn_delete(T, rl);
    btn_delete(T, rr);

    /* check p-is-root case in btn_replace_child */
    btn *c = btn_new(T);
    btn *s = btn_new(T);
    T->root = c;
    btn_replace_child(T, c->parent, c, s);
    test(T->root == s);
    test(s->parent == NULL);

    btn_delete(T, c);
    btn_delete(T, s);

    T->root = NULL;
    bt_delete(T);
    return 0;
}

int test_bt_copy(void)
{
    bt *T = bt_new(NONE, &str_type, &int_type);

    /* set up a source tree */
    str *kn  = str_from_cstr("root");
    str *kl  = str_from_cstr("left");
    str *kr  = str_from_cstr("right");
    str *krl = str_from_cstr("right-left");
    int vn  = 0;
    int vl  = 1;
    int vr  = 2;
    int vrl = 3;

    btn *n = btn_new(T);
    btn_set_key(T, n, kn);
    btn_set_value(T, n, &vn);

    btn *l = btn_new(T);
    btn_set_key(T, l, kl);
    btn_set_value(T, l, &vl);

    btn *r = btn_new(T);
    btn_set_key(T, r, kr);
    btn_set_value(T, r, &vr);

    btn *rl = btn_new(T);
    btn_set_key(T, rl, krl);
    btn_set_value(T, rl, &vrl);

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
    test(str_compare(btn_key  (T, c), kn)  == 0);
    test(int_compare(btn_value(T, c), &vn) == 0);

    test(cl);
    test(str_compare(btn_key  (T, cl), kl)  == 0);
    test(int_compare(btn_value(T, cl), &vl) == 0);
    test(cl->left == NULL && cl->right == NULL);

    test(cr);
    test(str_compare(btn_key  (T, cr), kr)  == 0);
    test(int_compare(btn_value(T, cr), &vr) == 0);
    test(cr->left != NULL && cr->right == NULL);

    test(crl);
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

int test_bt_has(void)
{
    bt *T = bt_new(NONE, &str_type, NULL);
    test(T);

    str *c = str_from_cstr("c");
    str *a = str_from_cstr("a");
    str *b = str_from_cstr("b");
    str *d = str_from_cstr("d");

    T->root = btn_new(T);
    T->root->parent = NULL;
    btn_set_key(T, T->root, c);

    T->root->left = btn_new(T);
    T->root->left->parent = T->root;
    btn_set_key(T, T->root->left, a);

    T->root->left->right = btn_new(T);
    T->root->left->right->parent = T->root->left;
    btn_set_key(T, T->root->left->right, b);

    T->count = 3;

    test(bt_has(T, c) == 1);
    test(bt_has(T, a) == 1);
    test(bt_has(T, b) == 1);
    test(bt_has(T, d) == 0);

    str_delete(a);
    str_delete(b);
    str_delete(c);
    str_delete(d);
    bt_delete(T);
    return 0;
}

int test_bt_insert(void)
{
    bt *T = bt_new(NONE, &int_type, NULL);

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

int test_bt_remove(void)
{
    bt *T = bt_new(NONE, &int_type, NULL);

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

    /* from here on it's just testing insert... */
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
int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* log_info("random seed was %u", seed); */

    run_test(test_node_handlers);
    run_test(test_node_rotations);
    run_test(test_bt_copy);
    run_test(test_bt_has);
    run_test(test_bt_insert);
    run_test(test_bt_remove);

    test_suite_end();
}
