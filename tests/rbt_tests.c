#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "rbt.h"
#include "test_utils.h"
#include "unittest.h"

#define MAX_VALUE 1000

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
    test(!__rbt_node_rotate_right(&T, n, &res), "__rbt_node_rotate_right failed.");
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
    test(!__rbt_node_rotate_left(&T, n, &res), "__rbt_node_rotate_right failed.");
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

int test_rbt_init(void)
{
    rc = __rbt_init(&T, sizeof(int), compint, NULL);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(T.root == NULL, "T.root = %p (%p)", T.root, NULL);
    test(T.element_size == sizeof(int), "T.element_size = %lu (%lu)",
            T.element_size, sizeof(int));
    test(T.size == 0, "T.size = %lu (%lu)", T.size, 0lu);
    test((void*)T.compare == (void*)compint, "T.compare != compint");
    test(T.destroy == NULL, "T.destroy = %p (%p)", T.destroy, NULL);

    return TEST_OK;
}

int test_rbt_usage(void)
{
    for (int i = 0; i < 64; ++i) {
        rc = __rbt_insert(&T, &i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
        test(T.size == (size_t)(i + 1), "T.size = %lu (%d)", T.size, i + 1);
    }

    for (int i = 0; i < 64; ++i) {
        int v = (int)rand() % MAX_VALUE;
        rc = __rbt_insert(&T, &v);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }
    return TEST_OK;
}

int test_rbt_clear(void)
{
    __rbt_clear(&T);
    return TEST_OK;
}

int main(void)
{
    srand((unsigned)time(NULL));
    test_suite_start();
    run_test(test_rotations);
    run_test(test_rbt_init);
    run_test(test_rbt_usage);
    run_test(test_rbt_clear);
    test_suite_end();
}
