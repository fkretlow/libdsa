#include "binary_tree.h"
#include "str.h"
#include "test.h"
#include "type_interface.h"

static bt T;

int test_node_handlers(void)
{
    bt_initialize(&T, NONE, &str_type, &int_type);

    btn *n = btn_new(&T);
    test(n);

    str *k1 = str_from_cstr("key");
    str *k2 = str_from_cstr("this is a very long key");

    btn_set_key(&T, n, k1);
    test(btn_has_key(n) == 1);
    test(str_compare(btn_get_key(&T, n), k1) == 0);

    btn_destroy_key(&T, n);
    test(btn_has_key(n) == 0);

    btn_set_key(&T, n, k2);
    test(btn_has_key(n) == 1);
    test(str_compare(btn_get_key(&T, n), k2) == 0);

    int v = 10;
    btn_set_value(&T, n, &v);
    test(btn_has_value(n) == 1);
    test(*(int*)btn_get_value(&T, n) == v);

    btn_delete(&T, n);
    bt_destroy(&T);
    str_delete(k1);
    str_delete(k2);
    return 0;
}

int test_node_rotations(void)
{
    bt_initialize(&T, NONE, &int_type, NULL);

    btn *p = btn_new(&T);
    btn *n = btn_new(&T);
    btn *l = btn_new(&T);
    btn *ll = btn_new(&T);
    btn *lr = btn_new(&T);
    btn *r = btn_new(&T);
    btn *rl = btn_new(&T);
    btn *rr = btn_new(&T);

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
    btn_rotate_right(&T, n, &res);
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
    btn_rotate_left(&T, n, &res);
    test(res == r);
    test(p->right == r);
    test(r->parent == p);
    test(r->right == rr);
    test(r->left == n);
    test(n->parent == r);
    test(n->right == rl);
    test(rl->parent == n);

    btn_delete(&T, p);
    btn_delete(&T, n);
    btn_delete(&T, l);
    btn_delete(&T, ll);
    btn_delete(&T, lr);
    btn_delete(&T, r);
    btn_delete(&T, rl);
    btn_delete(&T, rr);

    bt_destroy(&T);
    return 0;
}

int main(void)
{
    test_suite_start();

    run_test(test_node_handlers);
    run_test(test_node_rotations);

    test_suite_end();
}
