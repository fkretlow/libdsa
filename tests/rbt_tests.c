#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "rbt.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define MAX_VALUE 1000
#define N_VALUES 1000

static _rbt T;
static int rc;

struct rbt_stats {
    size_t red;
    size_t black;
    size_t full;
    size_t empty;
    size_t longest;
    size_t shortest;
};

int _rbt_node_get_stats(_rbt_node *n, void *stats)
{
    struct rbt_stats *s = stats;
    if (n->color == RED) {
        ++s->red;
    } else {
        ++s->black;
        if (n->color == BLACK
                && n->left && n->left->color == RED
                && n->right && n->right->color == RED) {
            ++s->full;
        } else if ( n->color == BLACK
                && (!n->left || n->left->color == BLACK)
                && (!n->right || n->right->color == BLACK)) {
            ++s->empty;
        }
    }

    if (!n->left || !n->right) {
        size_t l = 1;
        while (n->parent) {
            n = n->parent;
            ++l;
        }
        if (l < s->shortest) {
            s->shortest = l;
        } else if (l > s->longest) {
            s->longest = l;
        }
    }

    return 0;
}

void print_rbt_stats(const _rbt *T, const char *header)
{
    struct rbt_stats s = {
        .red = 0,
        .black = 0,
        .full = 0,
        .empty = 0,
        .longest = 0,
        .shortest = SIZE_MAX
    };
    _rbt_traverse(T, _rbt_node_get_stats, &s);

    printf("%s\n", "----------------------------------");
    if (header) printf("%s\n", header);
    printf("%-15s %5lu\n", "nodes", T->size);
    printf("%-15s %5lu   %5.2f%%\n",
            "red nodes", s.red, (double)s.red / T->size * 100.0);
    printf("%-15s %5lu   %5.2f%%\n",
            "full groups", s.full, (double)s.full / s.black * 100.0);
    printf("%-15s %5lu   %5.2f%%\n",
            "empty groups", s.empty, (double)s.empty / s.black * 100.0);
    printf("%-15s %5lu\n", "shortest path", s.shortest);
    printf("%-15s %5lu\n", "longest path", s.longest);
    printf("%s\n", "----------------------------------");
}

int print_node(_rbt_node *n, void *nothing)
{
    nothing = nothing;
    printf("%d %s\n", *(int*)n->data, n->color == RED ? "R" : "B");
    return 0;
}

int test_rotations(void)
{
    _rbt_node *p = _rbt_node_new();
    _rbt_node *n = _rbt_node_new();
    _rbt_node *l = _rbt_node_new();
    _rbt_node *ll = _rbt_node_new();
    _rbt_node *lr = _rbt_node_new();
    _rbt_node *r = _rbt_node_new();
    _rbt_node *rl = _rbt_node_new();
    _rbt_node *rr = _rbt_node_new();

    _rbt_node *res;

    test(p, "_rbt_node_new failed.");
    test(n, "_rbt_node_new failed.");
    test(l, "_rbt_node_new failed.");
    test(ll, "_rbt_node_new failed.");
    test(lr, "_rbt_node_new failed.");
    test(r, "_rbt_node_new failed.");
    test(rl, "_rbt_node_new failed.");
    test(rr, "_rbt_node_new failed.");

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


    v = -1;
    rc = _rbt_has(&T, &v);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    /* print_rbt_stats(&T, "sorted input"); */

    for (int i = 0; i < N_VALUES / 2; ++i) {
        rc = _rbt_remove(&T, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        rc = _rbt_has(&T, &v);
        test(rc == 0, "rc = %d (%d)" ,rc, 0);
    }

    _rbt_clear(&T);

    int values[N_VALUES] = { 0 };
    for (int i = 0; i < N_VALUES; ++i) {
        v = (int)rand() % MAX_VALUE;
        rc = _rbt_insert(&T, &v);
        test(rc >= 0, "_rbt_insert failed");
        if (rc == 1) {
            --i;
        } else {
            values[i] = v;
        }
        test(T.size == (size_t)i + 1, "T.size = %lu (%d)", T.size, i + 1);
        rc = _rbt_has(&T, &v);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    /* print_rbt_stats(&T, "unsorted input"); */
    /* _rbt_traverse(&T, print_node, NULL); */

    for (int i = 0; i < N_VALUES; ++i) {
        /* debug("deleting unsorted values: loop i = %d", i); */
        rc = _rbt_remove(&T, values + i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        rc = _rbt_has(&T, values + i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    _rbt_clear(&T);

    return TEST_OK;
}

int test_rbt_copy(void)
{
    for (int i = 0; i < 4; ++i) {
        _rbt_insert(&T, &i);
    }

    _rbt C = { 0 };
    int rc = _rbt_copy(&C, &T);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    _rbt_node *r = C.root;
    test(r->color == BLACK, "r->color = %d (%d)", r->color, BLACK);
    test(*(int*)r->data == 1, "r->data = %d (%d)", *(int*)r->data, 1);

    _rbt_node *rl = r->left;
    test(rl->color == BLACK, "rl->color = %d (%d)", rl->color, BLACK);
    test(*(int*)rl->data == 0, "rl->data = %d (%d)", *(int*)rl->data, 0);

    _rbt_node *rr = r->right;
    test(rr->color == BLACK, "rr->color = %d (%d)", rr->color, BLACK);
    test(*(int*)rr->data == 2, "rr->data = %d (%d)", *(int*)rr->data, 2);

    _rbt_node *rrr = rr->right;
    test(rrr->color == RED, "rrr->color = %d (%d)", rrr->color, RED);
    test(*(int*)rrr->data == 3, "rrr->data = %d (%d)", *(int*)rrr->data, 3);

    _rbt_clear(&C);
    _rbt_clear(&T);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();

    unsigned seed = (unsigned)time(NULL);
    /* unsigned seed = 1604388022; */
    srand(seed);
    /* debug("random seed was %u", seed); */

    run_test(test_rotations);
    run_test(test_rbt_init);
    run_test(test_rbt_usage);
    run_test(test_rbt_copy);
    test_suite_end();
}
