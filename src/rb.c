/*************************************************************************************************
 *
 * rb.c
 *
 * Algorithms for insertion into and deletion from a left-leaning red-black (2-3) tree (LLRB).
 *
 * For the most part this is a literal C translation of the original Java implementation by R.
 * Sedgewick (see https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf).
 *
 * The algorithms are called by the high level functions of the bst interface declared in bst.h if
 * the balancing strategy is set to RB for the tree they are called on. See the docstring in bst.c
 * for additional information.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include <string.h>
#include "bst.h"
#include "check.h"

#define rb_n_is_red(n) ((n) && (n)->flags.rb.color == RED)

/* static inline bst_n *rb_n_rotate_left  (bst_n **np)
 * static inline bst_n *rb_n_rotate_right (bst_n **np)
 * Normal tree rotations with RB color adjustments. The pointer at np is changed. */
static inline void rb_n_rotate_left(bst_n **np)
{
    bst_n *n = *np;
    assert(n->right);
    bst_n *r = n->right;
    n->right = r->left;
    r->left = n;
    r->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    *np = r;
}

static inline void rb_n_rotate_right(bst_n **np)
{
    bst_n *n = *np;
    assert(n->left);
    bst_n *l = n->left;
    n->left = l->right;
    l->right = n;
    l->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    *np = l;
}

/* static inline void rb_n_color_flip(bst_n *n)
 * Flip the colors of n and its children. Corresponds to moving the middle node of a 4-node up
 * into the parent node in a 2-3-4 tree. */
static inline void rb_n_color_flip(bst_n *n)
{
    assert(n && n->left && n->right);
    n->flags.rb.color        = !n->flags.rb.color;
    n->left->flags.rb.color  = !n->left->flags.rb.color;
    n->right->flags.rb.color = !n->right->flags.rb.color;
}

/* static inline void rb_n_repair(bst_n **np)
 * Repair the RB properties on the way up the recursive chain after insertion or deletion. The
 * pointer at np may be updated. */
static inline void rb_n_repair(bst_n **np)
{
    bst_n *n = *np;
    /* rotate right-leaning 3-nodes */
    if (rb_n_is_red(n->right) && !rb_n_is_red(n->left))       rb_n_rotate_left(&n);
    /* rotate left-leaning (unbalanced) 4-nodes */
    if (rb_n_is_red(n->left) && rb_n_is_red(n->left->left))   rb_n_rotate_right(&n);
    /* eliminate 4-nodes */
    if (rb_n_is_red(n->left) && rb_n_is_red(n->right))        rb_n_color_flip(n);
    *np = n;
}

/* int rb_n_insert(bst *T, bst_n **np, const void *k, const void *v)
 * Insert k and (if given) v into a left leaning red-black (2-3) tree. The pointer at np may be
 * changed. */
int rb_n_insert(bst *T,
               bst_n **np,       /* address of the link to this node in the parent node */
               const void *k,   /* the key to insert */
               const void *v)   /* the value to insert, may be NULL */
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    int rc;
    bst_n *n = *np;

    if (!n) {
        n = bst_n_new(T, k, v);
        check(n, "failed to create new node");
        *np = n;
        return 1;
    }

    int cmp = t_compare(T->key_type, k, bst_n_key(T, n));

    if (cmp < 0) {
        rc = rb_n_insert(T, &n->left, k, v);
    } else if (cmp > 0) {
        rc = rb_n_insert(T, &n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bst_n_set_value(T, n, v);
        rc = 0;
    }

    rb_n_repair(&n);
    *np = n;
    return rc;
error:
    return -1;
}

/* static inline void rb_n_move_red_left (bst_n **np)
 * static inline void rb_n_move_red_right(bst_n **np)
 * Helper functions for deletion: Ensure that the left/right child node of n is not a 2-node.
 * The pointer at np may be changed. */
static inline void rb_n_move_red_left(bst_n **np)
{
    bst_n *n = *np;
    assert(!rb_n_is_red(n->left) && !rb_n_is_red(n->right));

    rb_n_color_flip(n);
    if (rb_n_is_red(n->right->left)) {
        rb_n_rotate_right(&n->right);
        rb_n_rotate_left(&n);
        rb_n_color_flip(n);
    }
    *np = n;
}

static inline void rb_n_move_red_right(bst_n **np)
{
    bst_n *n = *np;
    assert(n->left && !rb_n_is_red(n->left) && !rb_n_is_red(n->right));

    rb_n_color_flip(n);
    if (rb_n_is_red(n->left->left)) {
        rb_n_rotate_right(&n);
        rb_n_color_flip(n);
    }
    *np = n;
}

/* int rb_n_remove_min(bst *T, bst_n **np)
 * Remove the minimum of the subtree with the root n. The pointer at np may be changed.
 * This always removes a node, but we return 1 whatsoever for consistency with rb_n_remove. */
int rb_n_remove_min(bst *T, bst_n **np)
{
    bst_n *n = *np;
    assert(n);

    int rc;

    if (!n->left) {
        /* The LLRB invariants imply that we can't have a right child if we don't have a left one:
         * No right child is red, and if we had a black one, the black-height property would be
         * violated. */
        assert(!n->right);
        bst_n_delete(T, n);
        *np = NULL;
        rc = 1;
    } else {
        /* Ensure the left child isn't a 2-node. */
        if (n->left && !rb_n_is_red(n->left) && !rb_n_is_red(n->left->left))  rb_n_move_red_left(&n);
        rc = rb_n_remove_min(T, &n->left);
        rb_n_repair(&n);
        *np = n;
    }

    return rc;
}

/* int rb_n_remove(bst *T, bst_n **np, const void *k)
 * Remove the node with the key k from the subtree with the root n, preserving the LLRB
 * invariants. The pointer at np may be changed.
 * Return 1 if a node was removed, 0 if k wasn't found, or -1 on error. */
int rb_n_remove(
        bst *T,
        bst_n **np,      /* the root of the subtree to delete from */
        const void *k)  /* the key to delete */
{
    assert(T && T->key_type && k);

    bst_n *n = *np;
    if (!n) return 0;

    int rc;

    if (t_compare(T->key_type, k, bst_n_key(T, n)) < 0) {
        if (!rb_n_is_red(n->left) && n->left && !rb_n_is_red(n->left->left)) {
            rb_n_move_red_left(&n);
        }
        rc = rb_n_remove(T, &n->left, k);
    }

    else {
        if (rb_n_is_red(n->left)) {
            rb_n_rotate_right(&n);
        }

        if (t_compare(T->key_type, k, bst_n_key(T, n)) == 0 && !n->right) {
            assert(!n->left);
            bst_n_delete(T, n);
            *np = NULL;
            return 1;
        }

        if (!rb_n_is_red(n->right) && n->right && !rb_n_is_red(n->right->left)) {
            rb_n_move_red_right(&n);
        }

        if (t_compare(T->key_type, k, bst_n_key(T, n)) == 0) {
            bst_n *s;
            /* Find the node with the minimum key in the right subtree, which is guaranteed to not
             * have a left child; move its data over here, then continue down the right subtree to
             * delete it. */
            s = n->right;
            while (s->left) s = s->left;
            bst_n_move_data(T, n, s);
            rc = rb_n_remove_min(T, &n->right);

        } else {
            rc = rb_n_remove(T, &n->right, k);
        }
    }

    rb_n_repair(&n);
    *np = n;
    return rc;
}

/* int rb_n_invariant(const bst *T, const bst_n *n, int depth, int black_depth, struct bst_stats *s)
 * Check if the LLRB invariants hold for the subtree with the root n and collect stats of the tree
 * while at it. */
int rb_n_invariant(
        const bst *T,
        const bst_n *n,
        int depth,              /* the depth of the parent */
        int black_depth,        /* the black depth of the parent */
        struct bst_stats *s)    /* where rolling stats are accumulated */
{
    if (!n) return 0;

    ++depth;
    ++s->total_nodes;
    if (rb_n_is_red(n)) { ++s->red_nodes; }
    else               { ++s->black_nodes; ++black_depth; }

    if (!n->left && !n->right) {
        if (!s->shortest_path || depth < s->shortest_path) s->shortest_path = depth;
        if (!s->height        || depth > s->height)        s->height = depth;
    }

    /* check key inequalities */
    if (n->left && t_compare(T->key_type, bst_n_key(T, n->left), bst_n_key(T, n)) >= 0) {
        log_error("BST invariant violated: left child > parent");
        return -1;
    }
    if (n->right && t_compare(T->key_type, bst_n_key(T, n->right), bst_n_key(T, n)) <= 0) {
        log_error("BST invariant violated: right child < parent");
        return -1;
    }

    /* check red links property */
    if (rb_n_is_red(n) && (rb_n_is_red(n->left) || rb_n_is_red(n->right))) {
        log_error("RB invariant violated: subsequent red nodes");
        return -2;
    }

    /* check for 4-nodes */
    if (rb_n_is_red(n->left) && rb_n_is_red(n->right)) {
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
    rc = rb_n_invariant(T, n->left, depth, black_depth, s);
    if (rc < 0) return rc;
    rc = rb_n_invariant(T, n->right, depth, black_depth, s);
    if (rc < 0) return rc;

    return 0;
}
