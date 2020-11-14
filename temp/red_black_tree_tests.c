#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "red_black_tree.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define MAX_VALUE 1000
#define N_VALUES 100

static RBTree T;
static int rc;

struct rbt_stats {
    size_t red;
    size_t black;
    size_t full;
    size_t empty;
    size_t longest;
    size_t shortest;
};

int RBTreeNode_get_stats(RBTreeNode *n, void *stats)
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

void print_tree_stats(RBTree *T, const char *header)
{
    struct rbt_stats s = {
        .red = 0,
        .black = 0,
        .full = 0,
        .empty = 0,
        .longest = 0,
        .shortest = SIZE_MAX
    };
    RBTree_traverse_nodes(T, RBTreeNode_get_stats, &s);

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

/* static int print_node(RBTreeNode *n, void *nothing)
{
    nothing = nothing;
    printf("%d %s\n", *(int*)n->key, n->color == RED ? "R" : "B");
    return 0;
} */

int test_rotations(void)
{
    RBTreeNode *p = RBTreeNode_new();
    RBTreeNode *n = RBTreeNode_new();
    RBTreeNode *l = RBTreeNode_new();
    RBTreeNode *ll = RBTreeNode_new();
    RBTreeNode *lr = RBTreeNode_new();
    RBTreeNode *r = RBTreeNode_new();
    RBTreeNode *rl = RBTreeNode_new();
    RBTreeNode *rr = RBTreeNode_new();

    RBTreeNode *res;

    test(p, "RBTreeNode_new failed.");
    test(n, "RBTreeNode_new failed.");
    test(l, "RBTreeNode_new failed.");
    test(ll, "RBTreeNode_new failed.");
    test(lr, "RBTreeNode_new failed.");
    test(r, "RBTreeNode_new failed.");
    test(rl, "RBTreeNode_new failed.");
    test(rr, "RBTreeNode_new failed.");

    // construct the test tree
    p->left = n, n->parent = p;
    n->left = l, l->parent = n;
    l->left = ll, ll->parent = l;
    l->right = lr, lr->parent = l;
    n->right = r, r->parent = n;
    r->left = rl, rl->parent = r;
    r->right = rr, rr->parent = r;

    // rotate right and check
    test(!RBTreeNode_rotate_right(&T, n, &res), "RBTreeNode_rotate_right failed.");
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
    test(!RBTreeNode_rotate_left(&T, n, &res), "RBTreeNode_rotate_right failed.");
    test(res == r, "res != l");
    test(p->right == r, "p->right != r");
    test(r->parent == p, "r->parent != p");
    test(r->right == rr, "r->right != rr");
    test(r->left == n, "r->left != n");
    test(n->parent == r, "n->parent != r");
    test(n->right == rl, "n->right != rl");
    test(rl->parent == n, "rl->parent != n");

    /* Lazy fix: The node destructors need a key type in some situations... */
    T.key_type = &int_type;
    RBTreeNode_delete(&T, p);
    RBTreeNode_delete(&T, n);
    RBTreeNode_delete(&T, l);
    RBTreeNode_delete(&T, ll);
    RBTreeNode_delete(&T, lr);
    RBTreeNode_delete(&T, r);
    RBTreeNode_delete(&T, rl);
    RBTreeNode_delete(&T, rr);

    return TEST_OK;
}

int test_rbtree_initialize(void)
{
    rc = RBTree_initialize(&T, &int_type, NULL);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(T.external_storage == 0, "T has storage allocated when it shouldn't.");
    test(T.root == NULL, "T.root = %p (%p)", T.root, NULL);
    test(T.key_type->size == sizeof(int), "T.key_type->size = %lu (%lu)",
            T.key_type->size, sizeof(int));
    test(T.value_type == NULL, "T.value_type != NULL");
    test(T.size == 0, "T.size = %lu (%lu)", T.size, 0lu);

    return TEST_OK;
}

int test_rbtree_usage(void)
{
    int v;

    size_t size = 0;
    for (int i = 0; i < N_VALUES; ++i) {
        rc = RBTree_insert(&T, &i);
        ++size;
        test(rc == 0, "rc = %d (%d)", rc, 0);
        test(T.size == size, "T.size = %lu (%lu)", T.size, size);
        rc = RBTree_has(&T, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }


    v = -1;
    rc = RBTree_has(&T, &v);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    /* print_tree_stats(&T, "sorted input"); */

    for (int i = 0; i < N_VALUES / 2; ++i) {
        rc = RBTree_remove(&T, &i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        rc = RBTree_has(&T, &v);
        test(rc == 0, "rc = %d (%d)" ,rc, 0);
    }

    RBTree_clear(&T);

    int values[N_VALUES] = { 0 };
    for (int i = 0; i < N_VALUES; ++i) {
        v = (int)rand() % MAX_VALUE;
        rc = RBTree_insert(&T, &v);
        test(rc >= 0, "RBTree_insert failed");
        if (rc == 1) {
            --i;
        } else {
            values[i] = v;
        }
        test(T.size == (size_t)i + 1, "T.size = %lu (%d)", T.size, i + 1);
        rc = RBTree_has(&T, &v);
        test(rc == 1, "rc = %d (%d)", rc, 1);
    }

    /* print_tree_stats(&T, "unsorted input"); */
    /* RBTree_traverse(&T, print_node, NULL); */

    for (int i = 0; i < N_VALUES; ++i) {
        /* debug("deleting unsorted values: loop i = %d", i); */
        rc = RBTree_remove(&T, values + i);
        test(rc == 1, "rc = %d (%d)", rc, 1);
        rc = RBTree_has(&T, values + i);
        test(rc == 0, "rc = %d (%d)", rc, 0);
    }

    RBTree_clear(&T);

    return TEST_OK;
}

int test_rbtree_copy(void)
{
    for (int i = 0; i < 4; ++i) {
        RBTree_insert(&T, &i);
    }

    RBTree C = { 0 };
    int rc = RBTree_copy(&C, &T);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    RBTreeNode *r = C.root;
    test(r->color == BLACK, "r->color = %d (%d)", r->color, BLACK);
    test(*(int*)RBTreeNode_key_address(&C, r) == 1,
            "r->key = %d (%d)", *(int*)RBTreeNode_key_address(&C, r), 1);

    RBTreeNode *rl = r->left;
    test(rl->color == BLACK, "rl->color = %d (%d)", rl->color, BLACK);
    test(*(int*)RBTreeNode_key_address(&C, rl) == 0,
            "rl->key = %d (%d)", *(int*)RBTreeNode_key_address(&C, rl), 0);

    RBTreeNode *rr = r->right;
    test(rr->color == BLACK, "rr->color = %d (%d)", rr->color, BLACK);
    test(*(int*)RBTreeNode_key_address(&C, rr) == 2,
            "rr->key = %d (%d)", *(int*)RBTreeNode_key_address(&C, rr), 2);

    RBTreeNode *rrr = rr->right;
    test(rrr->color == RED, "rrr->color = %d (%d)", rrr->color, RED);
    test(*(int*)RBTreeNode_key_address(&C, rrr) == 3,
            "rrr->key = %d (%d)", *(int*)RBTreeNode_key_address(&C, rrr), 3);

    RBTree_clear(&C);
    RBTree_clear(&T);
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
    run_test(test_rbtree_initialize);
    run_test(test_rbtree_usage);
    run_test(test_rbtree_copy);
    test_suite_end();
}
