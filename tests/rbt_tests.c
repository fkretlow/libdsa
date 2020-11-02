#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "rbt.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define MAX_VALUE 1024
#define N_VALUES 1024

static _rbt T;
static int rc;

static TypeInterface int_type = {
    sizeof(int),
    NULL,
    NULL,
    int_compare,
    NULL
};
int test_rotations(void)
{
    _rbt_node *p, *n, *l, *ll, *lr, *r, *rl, *rr, *res;
    test(!_rbt_node_new(&p), "_rbt_node_new failed.");
    test(!_rbt_node_new(&n), "_rbt_node_new failed.");
    test(!_rbt_node_new(&l), "_rbt_node_new failed.");
    test(!_rbt_node_new(&ll), "_rbt_node_new failed.");
    test(!_rbt_node_new(&lr), "_rbt_node_new failed.");
    test(!_rbt_node_new(&r), "_rbt_node_new failed.");
    test(!_rbt_node_new(&rl), "_rbt_node_new failed.");
    test(!_rbt_node_new(&rr), "_rbt_node_new failed.");

    // construct the test tree
    p->left = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    // rotate right and check
    test(!_rbt_node_rotate_right(&T, n, &res), "_rbt_node_rotate_right failed.");
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
    test(!_rbt_node_rotate_left(&T, n, &res), "_rbt_node_rotate_right failed.");
    test(res == r, "res != l");
    test(p->right == r, "p->right != r");
    test(r->parent == p, "r->parent != p");
    test(r->right == rr, "r->right != rr");
    test(r->left == n, "r->left != n");
    test(n->parent == r, "n->parent != r");
    test(n->right == rl, "n->right != rl");
    test(rl->parent == n, "rl->parent != n");

    _rbt_node_delete(&T, p);
    _rbt_node_delete(&T, n);
    _rbt_node_delete(&T, l);
    _rbt_node_delete(&T, ll);
    _rbt_node_delete(&T, lr);
    _rbt_node_delete(&T, r);
    _rbt_node_delete(&T, rl);
    _rbt_node_delete(&T, rr);

    return TEST_OK;
}

int test_rbt_init(void)
{
    rc = _rbt_init(&T, &int_type);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(T.root == NULL, "T.root = %p (%p)", T.root, NULL);
    test(T.element_type->size == sizeof(int), "T.element_type->size = %lu (%lu)",
            T.element_type->size, sizeof(int));
    test(T.size == 0, "T.size = %lu (%lu)", T.size, 0lu);

    return TEST_OK;
}

int test_rbt_usage(void)
{
    int v;

    size_t size = 0;
    for (int i = 0; i < N_VALUES; ++i) {
        rc = _rbt_insert(&T, &i);
        ++size;
        test(rc == 0, "rc = %d (%d)", rc, 0);
        test(T.size == size, "T.size = %lu (%lu)", T.size, size);
        rc = _rbt_has(&T, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    v = 5000;
    rc = _rbt_has(&T, &v);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    for (int i = 0; i < N_VALUES / 2; ++i) {
        rc = _rbt_remove(&T, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        rc = _rbt_has(&T, &v);
        test(rc == 0, "rc = %d (%d)" ,rc, 0);
    }

    _rbt_clear(&T);

    for (int i = 0; i < N_VALUES; ++i) {
        v = (int)rand() % MAX_VALUE;
        rc = _rbt_insert(&T, &v);
        test(rc >= 0, "_rbt_insert failed");
        if (rc == 1) { /* The value was already there. */
            --i;
        }
        test(T.size == (size_t)i + 1, "T.size = %lu (%d)", T.size, i + 1);
        rc = _rbt_has(&T, &v);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    _rbt_clear(&T);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_rotations);
    run_test(test_rbt_init);

    unsigned seed = (unsigned)time(NULL);
    srand(seed);
    /* debug("random seed was %u", seed); */
    /* srand(1604071123); */

    run_test(test_rbt_usage);
    test_suite_end();
}
