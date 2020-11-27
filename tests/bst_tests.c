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

int test_node_handlers(void)
{
    bst *T = bst_new(NONE, &str_type, &int_type);
    test(T);

    bst_node *n = calloc(1, bst_node_size(T));
    test(n);

    str *k1 = str_from_cstr("key");
    str *k2 = str_from_cstr("this is a very long key");

    bst_node_set_key(T, n, k1);
    test(bst_node_has_key(n) == 1);
    test(str_compare(bst_node_key(T, n), k1) == 0);

    bst_node_destroy_key(T, n);
    test(bst_node_has_key(n) == 0);

    bst_node_set_key(T, n, k2);
    test(bst_node_has_key(n) == 1);
    test(str_compare(bst_node_key(T, n), k2) == 0);

    int v = 10;
    bst_node_set_value(T, n, &v);
    test(bst_node_has_value(n) == 1);
    test(*(int*)bst_node_value(T, n) == v);

    bst_node_destroy_value(T, n);
    test(bst_node_has_value(n) == 0);

    bst_node_delete(T, n);
    bst_delete(T);
    str_delete(k1);
    str_delete(k2);
    return 0;
}

/* int test_node_rotations(void)
{
    bst *T = bst_new(NONE, &int_type, NULL);

    bst_node *p = bst_node_new(T);
    bst_node *n = bst_node_new(T);
    bst_node *l = bst_node_new(T);
    bst_node *ll = bst_node_new(T);
    bst_node *lr = bst_node_new(T);
    bst_node *r = bst_node_new(T);
    bst_node *rl = bst_node_new(T);
    bst_node *rr = bst_node_new(T);

    bst_node *res;

    test(p);
    test(n);
    test(l);
    test(ll);
    test(lr);
    test(r);
    test(rl);
    test(rr);

    [>construct the test tree<]
    p->left = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    [>rotate right and check<]
    bst_node_rotate_right(T, n, &res);
    test(res == l);
    test(p->left == l);
    test(l->parent == p);
    test(l->left == ll);
    test(l->right == n);
    test(n->parent == l);
    test(n->left == lr);
    test(lr->parent == n);

    [>reset the test tree<]
    p->left = NULL, p->right = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    [>rotate left and check<]
    bst_node_rotate_left(T, n, &res);
    test(res == r);
    test(p->right == r);
    test(r->parent == p);
    test(r->right == rr);
    test(r->left == n);
    test(n->parent == r);
    test(n->right == rl);
    test(rl->parent == n);

    bst_node_delete(T, p);
    bst_node_delete(T, n);
    bst_node_delete(T, l);
    bst_node_delete(T, ll);
    bst_node_delete(T, lr);
    bst_node_delete(T, r);
    bst_node_delete(T, rl);
    bst_node_delete(T, rr);

    [>check p-is-root case in bst_node_replace_child<]
    bst_node *c = bst_node_new(T);
    bst_node *s = bst_node_new(T);
    T->root = c;
    bst_node_replace_child(T, c->parent, c, s);
    test(T->root == s);
    test(s->parent == NULL);

    bst_node_delete(T, c);
    bst_node_delete(T, s);

    T->root = NULL;
    bst_delete(T);
    return 0;
}
 */
int test_bst_copy(void)
{
    bst *T = bst_new(NONE, &str_type, &int_type);

    /* set up a source tree */
    str *kn  = str_from_cstr("1 root");
    str *kl  = str_from_cstr("0 left");
    str *kr  = str_from_cstr("3 right");
    str *krl = str_from_cstr("2 right-left");
    int vn  = 0;
    int vl  = 1;
    int vr  = 2;
    int vrl = 3;

    bst_node *n = bst_node_new(T, kn, &vn);
    bst_node *l = bst_node_new(T, kl, &vl);
    bst_node *r = bst_node_new(T, kr, &vr);
    bst_node *rl = bst_node_new(T, krl, &vrl);

    T->root = n;
    T->count = 4;
    n->left = l; l->parent = n;
    n->right = r; r->parent = n;
    r->left = rl; rl->parent = r;

    /* copy and verify */
    bst *C = bst_copy(T);
    test(C->count == T->count);
    test(C->key_type == T->key_type);
    test(C->value_type == T->value_type);

    bst_node *c = C->root;
    bst_node *cl = c->left;
    bst_node *cr = c->right;
    bst_node *crl = c->right->left;

    test(c);
    test(str_compare(bst_node_key  (T, c), kn)  == 0);
    test(int_compare(bst_node_value(T, c), &vn) == 0);

    test(cl);
    test(str_compare(bst_node_key  (T, cl), kl)  == 0);
    test(int_compare(bst_node_value(T, cl), &vl) == 0);
    test(cl->left == NULL && cl->right == NULL);

    test(cr);
    test(str_compare(bst_node_key  (T, cr), kr)  == 0);
    test(int_compare(bst_node_value(T, cr), &vr) == 0);
    test(cr->left != NULL && cr->right == NULL);

    test(crl);
    test(str_compare(bst_node_key  (T, crl), krl)  == 0);
    test(int_compare(bst_node_value(T, crl), &vrl) == 0);
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

int test_bst_has(void)
{
    bst *T = bst_new(NONE, &str_type, NULL);
    test(T);

    str *c = str_from_cstr("c");
    str *a = str_from_cstr("a");
    str *b = str_from_cstr("b");
    str *d = str_from_cstr("d");

    T->root = bst_node_new(T, c, NULL);
    T->root->parent = NULL;

    T->root->left = bst_node_new(T, a, NULL);
    T->root->left->parent = T->root;

    T->root->left->right = bst_node_new(T, b, NULL);
    T->root->left->right->parent = T->root->left;

    T->count = 3;

    test(bst_has(T, c) == 1);
    test(bst_has(T, a) == 1);
    test(bst_has(T, b) == 1);
    test(bst_has(T, d) == 0);

    str_delete(a);
    str_delete(b);
    str_delete(c);
    str_delete(d);
    bst_delete(T);
    return 0;
}

int test_bst_insert(void)
{
    bst *T = bst_new(NONE, &int_type, NULL);

    int rc, i, v;
    int values[NMEMB] = { 0 };
    uint32_t count = 0;

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_insert(T, &i);
        ++count;
        test(rc >= 0);
        test(bst_count(T) == count);
        test(bst_has(T, &i) == 1);
    }

    v = 0;
    rc = bst_insert(T, &v);
    test(rc == 0);

    bst_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bst_insert(T, &v);
        test(rc >= 0);
        values[i] = v;
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_has(T, values + i);
        test(rc == 1);
    }

    bst_delete(T);
    return 0;
}

int test_bst_remove(void)
{
    bst *T = bst_new(NONE, &int_type, NULL);

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

    /* from here on it's just testing insert... */
    v = 0;
    rc = bst_remove(T, &v);
    test(rc == 0);

    bst_clear(T);
    count = 0;

    for (i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        rc = bst_insert(T, &v);
        test(rc >= 0);
        values[i] = v;
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_remove(T, values + i);
        test(rc >= 0);
    }

    test(T->root == NULL);

    bst_delete(T);
    return 0;
}

int test_bst_set_get(void)
{
    bst *T = bst_new(NONE, &str_type, &int_type);
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
        rc = bst_set(T, keys[i], &values[i]);
        test(rc == 0);
    }

    for (i = 0; i < NMEMB; ++i) {
        v = bst_get(T, keys[i]);
        test(v != NULL);
        test(*v == values[i]);
    }

    for (i = 0; i < NMEMB; ++i) {
        rc = bst_remove(T, keys[i]);
        test(rc >= 0);
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

    run_test(test_node_handlers);
    /* run_test(test_node_rotations); */
    run_test(test_bst_copy);
    run_test(test_bst_has);
    run_test(test_bst_insert);
    run_test(test_bst_remove);
    run_test(test_bst_set_get);

    test_suite_end();
}
