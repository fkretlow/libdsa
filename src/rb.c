/*************************************************************************************************
 *
 * rb.c
 *
 * Algorithms for insertion into and deletion from a left-leaning red-black (2-3) tree (LLRB).
 *
 * For the most part this is a literal C translation of the original Java implementation by R.
 * Sedgewick (see https://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf).
 *
 * Sedgewick's code is elegant in that it can replace changed child links with returns and simple
 * assignments: `node = insert(node, key)`. Here, in contrast, creating a new node requires a call
 * to `malloc`. That can fail, so we need a way to signal failure up the call chain. As everywhere
 * else in the library this is done with integer return codes and it also provides a way to inform
 * the caller if the number of nodes has changed. In order to be able to update child links,
 * pointers to those links (pointers) are passed instead of the links themselves: `int rc =
 * insert(tree, &node, key)`.
 *
 * The algorithms are called by the high level functions of the bst interface declared in bst.h if
 * the balancing strategy is set to RB for the tree they are called on.
 *
 * Author: Florian Kretlow, 2020
 * Use, modify, and distribute as you wish.
 *
 ************************************************************************************************/

#include <assert.h>
#include <string.h>
#include "bst.h"
#include "check.h"

#define rbn_is_red(n) ((n) && (n)->flags.rb.color == RED)

/* static inline bstn *rbn_rotate_left  (bstn **np)
 * static inline bstn *rbn_rotate_right (bstn **np)
 * Normal tree rotations with RB color adjustments. The pointer at np is changed. */
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

/* static inline void rbn_repair(bstn **np)
 * Repair the RB properties on the way up the recursive chain after insertion or deletion. The
 * pointer at np may be updated. */
static inline void rbn_repair(bstn **np)
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
 * Insert k and (if given) v into a left leaning red-black (2-3) tree. The pointer at np may be
 * changed. */
int rbn_insert(bst *T,
               bstn **np,       /* address of the link to this node in the parent node */
               const void *k,   /* the key to insert */
               const void *v)   /* the value to insert, may be NULL */
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

    rbn_repair(&n);
    *np = n;
    return rc;
error:
    return -1;
}

/* static inline void rbn_move_red_left (bstn **np)
 * static inline void rbn_move_red_right(bstn **np)
 * Helper functions for deletion: Ensure that the left/right child node of n is not a 2-node.
 * The pointer at np may be changed. */
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
    assert(n->left && !rbn_is_red(n->left) && !rbn_is_red(n->right));

    rbn_color_flip(n);
    if (rbn_is_red(n->left->left)) {
        rbn_rotate_right(&n);
        rbn_color_flip(n);
    }
    *np = n;
}

/* int rbn_remove_min(bst *T, bstn **np)
 * Remove the minimum of the subtree with the root n. The pointer at np may be changed.
 * This always removes a node, but we return 1 whatsoever for consistency with rbn_remove. */
int rbn_remove_min(bst *T, bstn **np)
{
    bstn *n = *np;
    assert(n);

    int rc;

    if (!n->left) {
        /* The LLRB invariants imply that we can't have a right child if we don't have a left one:
         * No right child is red, and if we had a black one, the black-height property would be
         * violated. */
        assert(!n->right);
        bstn_delete(T, n);
        *np = NULL;
        rc = 1;
    } else {
        /* Ensure the left child isn't a 2-node. */
        if (n->left && !rbn_is_red(n->left) && !rbn_is_red(n->left->left))  rbn_move_red_left(&n);
        rc = rbn_remove_min(T, &n->left);
        rbn_repair(&n);
        *np = n;
    }

    return rc;
}

/* int rbn_remove(bst *T, bstn **np, const void *k)
 * Remove the node with the key k from the subtree with the root n, preserving the LLRB
 * invariants. The pointer at np may be changed.
 * Return 1 if a node was removed, 0 if k wasn't found, or -1 on error. */
int rbn_remove(
        bst *T,
        bstn **np,      /* the root of the subtree to delete from */
        const void *k)  /* the key to delete */
{
    assert(T && T->key_type && k);

    bstn *n = *np;
    if (!n) return 0;

    int rc;

    if (t_compare(T->key_type, k, bstn_key(T, n)) < 0) {
        if (!rbn_is_red(n->left) && n->left && !rbn_is_red(n->left->left)) {
            rbn_move_red_left(&n);
        }
        rc = rbn_remove(T, &n->left, k);
    }

    else {
        if (rbn_is_red(n->left)) {
            rbn_rotate_right(&n);
        }

        if (t_compare(T->key_type, k, bstn_key(T, n)) == 0 && !n->right) {
            assert(!n->left);
            bstn_delete(T, n);
            *np = NULL;
            return 1;
        }

        if (!rbn_is_red(n->right) && n->right && !rbn_is_red(n->right->left)) {
            rbn_move_red_right(&n);
        }

        if (t_compare(T->key_type, k, bstn_key(T, n)) == 0) {
            bstn *s;
            /* find the node with the minimum key in the right subtree, which is guaranteed to not
             * have a left child; move its data over here, then continue down the right subtree to
             * delete it */
            s = n->right;
            while (s->left) s = s->left;
            bstn_move_data(T, n, s);
            rc = rbn_remove_min(T, &n->right);

        } else {
            rc = rbn_remove(T, &n->right, k);
        }
    }

    rbn_repair(&n);
    *np = n;
    return rc;
}

/* int rbn_invariant(const bst *T, const bstn *n, int depth, int black_depth, struct bst_stats *s)
 * Check if the LLRB invariants hold for the subtree with the root n and collect stats of the tree
 * while at it. */
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
