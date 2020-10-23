#include "debug.h"
#include "rbt.h"
#include "unittest.h"

static int rc;
static __rbt T;

int test_rotations(void)
{
    __rbt_node *p, *n, *l, *ll, *lr, *r, *rl, *rr, *res;
    test(!__rbt_node_new(&p), "__rbt_node_new failed.");
    test(!__rbt_node_new(&n), "__rbt_node_new failed.");
    test(!__rbt_node_new(&l), "__rbt_node_new failed.");
    test(!__rbt_node_new(&ll), "__rbt_node_new failed.");
    test(!__rbt_node_new(&lr), "__rbt_node_new failed.");
    test(!__rbt_node_new(&r), "__rbt_node_new failed.");
    test(!__rbt_node_new(&rl), "__rbt_node_new failed.");
    test(!__rbt_node_new(&rr), "__rbt_node_new failed.");

    // construct the test tree
    p->left = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    // rotate right and check
    test(!__rbt_node_rotate_right(n, &res), "__rbt_node_rotate_right failed.");
    test(res == l, "res != l");
    test(p->left == l, "p->left != l");
    test(l->parent == p, "l->parent != p");
    test(l->left == ll, "l->left != ll");
    test(l->right == n, "l->right != n");
    test(n->parent == l, "n->parent != l");
    test(n->left == lr, "n->left != lr");
    test(lr->parent == n, "lr->parent != n");

    // reset the test tree
    p->left = NULL, p->right = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    // rotate left and check
    test(!__rbt_node_rotate_left(n, &res), "__rbt_node_rotate_right failed.");
    test(res == r, "res != l");
    test(p->right == r, "p->right != r");
    test(r->parent == p, "r->parent != p");
    test(r->right == rr, "r->right != rr");
    test(r->left == n, "r->left != n");
    test(n->parent == r, "n->parent != r");
    test(n->right == rl, "n->right != rl");
    test(rl->parent == n, "rl->parent != n");

    __rbt_node_delete(&T, p);
    __rbt_node_delete(&T, n);
    __rbt_node_delete(&T, l);
    __rbt_node_delete(&T, ll);
    __rbt_node_delete(&T, lr);
    __rbt_node_delete(&T, r);
    __rbt_node_delete(&T, rl);
    __rbt_node_delete(&T, rr);

    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_rotations);
    test_suite_end();
}
