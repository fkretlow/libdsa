/************************************************************************************************
 * rb.c
 * Algorithms for insertion into and deletion from a left-leaning red-black (2-3) tree (LLRB).
 * Mostly a literal C translation of the original Java implementations by Robert Sedgewick.
 ***********************************************************************************************/

#include <assert.h>
#include <string.h>
#include "bst.h"
#include "check.h"

#define rbn_is_red(n) ((n) && (n)->flags.rb.color == RED)

/* static inline bstn *rbn_rotate_left  (bstn *n)
 * static inline bstn *rbn_rotate_right (bstn *n)
 * Classic tree rotations with color adjustments. */
static inline void rbn_rotate_left(bstn **np)
{
    bstn *n = *np;
    assert(n->right);
    bstn *r = n->right;
    n->right = r->left;
    r->left = n;
    r->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    *np = r;
}

static inline void rbn_rotate_right(bstn **np)
{
    bstn *n = *np;
    assert(n->left);
    bstn *l = n->left;
    n->left = l->right;
    l->right = n;
    l->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    *np = l;
}

/* static inline void rbn_color_flip(bstn *n)
 * Flip the colors of n and its children. Corresponds to moving the middle node of a 4-node up
 * into the parent node in a 2-3-4 tree. */
static inline void rbn_color_flip(bstn *n)
{
    assert(n && n->left && n->right);
    n->flags.rb.color        = !n->flags.rb.color;
    n->left->flags.rb.color  = !n->left->flags.rb.color;
    n->right->flags.rb.color = !n->right->flags.rb.color;
}

/* static inline void rbn_fix_up(bstn **np)
 * Fix the red-black tree on the way up the recursive chain after insertion or deletion. */
static inline void rbn_fix_up(bstn **np)
{
    bstn *n = *np;
    /* rotate right-leaning 3-nodes */
    if (rbn_is_red(n->right) && !rbn_is_red(n->left))       rbn_rotate_left(&n);
    /* rotate left-leaning (unbalanced) 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->left->left))   rbn_rotate_right(&n);
    /* eliminate 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->right))        rbn_color_flip(n);
    *np = n;
}

/* int rbn_insert(bst *T, bstn **np, const void *k, const void *v)
 * Insertion into a left leaning red-black (2-3) tree. v can be NULL. */
int rbn_insert(bst *T, bstn **np, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    int rc;
    bstn *n = *np;

    if (!n) {
        n = bstn_new(T, k, v);
        check(n, "failed to create new node");
        *np = n;
        return 1;
    }

    int cmp = t_compare(T->key_type, k, bstn_key(T, n));

    if (cmp < 0) {
        rc = rbn_insert(T, &n->left, k, v);
    } else if (cmp > 0) {
        rc = rbn_insert(T, &n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bstn_set_value(T, n, v);
        rc = 0;
    }

    rbn_fix_up(&n);
    *np = n;
    return rc;
error:
    return -1;
}

static inline void rbn_move_red_left(bstn **np)
{
    bstn *n = *np;
    assert(!rbn_is_red(n->left) && !rbn_is_red(n->right));

    rbn_color_flip(n);
    if (rbn_is_red(n->right->left)) {
        rbn_rotate_right(&n->right);
        rbn_rotate_left(&n);
        rbn_color_flip(n);
    }

    *np = n;
}

static inline void rbn_move_red_right(bstn **np)
{
    bstn *n = *np;
    assert(!rbn_is_red(n->left) && !rbn_is_red(n->right));

    rbn_color_flip(n);
    if (rbn_is_red(n->left->left)) {
        rbn_rotate_right(&n);
        rbn_color_flip(n);
    }

    *np = n;
}

int rbn_remove_min(bst *T, bstn **np)
{
    bstn *n = *np;
    if (!n) return 0;

    if (n->left) {
        if (!rbn_is_red(n->left) && n->left && !rbn_is_red(n->left->left)) {
            rbn_move_red_left(&n);
        }
        return rbn_remove_min(T, &n->left);
    } else {
        ...
    }
}

/* int rbn_invariant(const bst *T, const bstn *n, int depth, int black_depth, struct bst_stats *s)
 * Check if the invariants hold. */
int rbn_invariant(
        const bst *T,
        const bstn *n,
        int depth,              /* the depth of the parent */
        int black_depth,        /* the black depth of the parent */
        struct bst_stats *s)    /* where rolling stats are accumulated */
{
    if (!n) return 0;

    ++depth;
    ++s->total_nodes;
    if (rbn_is_red(n)) { ++s->red_nodes; }
    else               { ++s->black_nodes; ++black_depth; }

    if (!n->left && !n->right) {
        if (!s->shortest_path || depth < s->shortest_path) s->shortest_path = depth;
        if (!s->height        || depth > s->height)        s->height = depth;
    }

    /* check key inequalities */
    if (n->left && t_compare(T->key_type, bstn_key(T, n->left), bstn_key(T, n)) >= 0) {
        log_error("BST invariant violated: left child > parent");
        return -1;
    }
    if (n->right && t_compare(T->key_type, bstn_key(T, n->right), bstn_key(T, n)) <= 0) {
        log_error("BST invariant violated: right child < parent");
        return -1;
    }

    /* check red links property */
    if (rbn_is_red(n) && (rbn_is_red(n->left) || rbn_is_red(n->right))) {
        log_error("RB invariant violated: subsequent red nodes");
        return -2;
    }

    /* check for 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->right)) {
        log_error("RB invariant violated: 4-node in a 2-3 tree");
        return -3;
    }

    /* check black height propery */
    if (!n->left || !n->right) {
        if (!s->black_height) {
            s->black_height = black_depth;
        } else if (s->black_height != black_depth) {
            log_error("RB invariant violated: inconsistent black height");
            return -4;
        }
    }

    /* process children */
    int rc;
    rc = rbn_invariant(T, n->left, depth, black_depth, s);
    if (rc < 0) return rc;
    rc = rbn_invariant(T, n->right, depth, black_depth, s);
    if (rc < 0) return rc;

    return 0;
}
