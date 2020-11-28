/************************************************************************************************
 * rb.c
 * Algorithms for insertion into and deletion from a left-leaning red-black (2-3) tree (LLRB).
 * Mostly a literal C translation of the original Java implementations by Robert Sedgewick.
 ***********************************************************************************************/

#include <assert.h>
#include "bst.h"
#include "check.h"

#define rbn_is_red(n) ((n) && (n)->flags.rb.color == RED)

/* static inline bstn *rbn_rotate_left  (bstn *n)
 * static inline bstn *rbn_rotate_right (bstn *n)
 * Classic tree rotations with color adjustments. */
static inline bstn *rbn_rotate_left(bstn *n)
{
    assert(n->right);
    bstn *r = n->right;
    n->right = r->left;
    r->left = n;
    r->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    return r;
}

static inline bstn *rbn_rotate_right(bstn *n)
{
    assert(n->left);
    bstn *l = n->left;
    n->left = l->right;
    l->right = n;
    l->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    return l;
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

/* static inline bstn *rbn_fix_up(bstn *n)
 * Fix the red-black tree on the way up the recursive chain after insertion or deletion. */
static inline bstn *rbn_fix_up(bstn *n)
{
    /* rotate right-leaning 3-nodes */
    if (rbn_is_red(n->right) && !rbn_is_red(n->left))       n = rbn_rotate_left(n);
    /* rotate left-leaning (unbalanced) 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->left->left))   n = rbn_rotate_right(n);
    /* eliminate 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->right))        rbn_color_flip(n);
    return n;
}

/* bstn *rbn_insert(bst *T, bstn *n, const void *k, const void *v)
 * Insertion into a left leaning red-black (2-3) tree. v can be NULL. */
bstn *rbn_insert(bst *T, bstn *n, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    if (!n) {
        ++T->count;
        return bstn_new(T, k, v);
    }

    int cmp = t_compare(T->key_type, k, bstn_key(T, n));

    if (cmp < 0) {
        n->left = rbn_insert(T, n->left, k, v);
    } else if (cmp > 0) {
        n->right = rbn_insert(T, n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bstn_set_value(T, n, v);
    }

    return rbn_fix_up(n);
}

/* int rbn_analyze(bstn *n, int depth, int black_depth, struct rb_stats *s)
 * int rb_analyze(const bst *T, struct rb_stats *s)
 * Analyze a LLRB and store the results in the given struct. */
int rbn_analyze(bstn *n, int depth, int black_depth, struct rb_stats *s)
{
    if (!n) return 0;

    ++depth;
    if (rbn_is_red(n)) { ++s->red_nodes; }
    else               { ++s->black_nodes; ++black_depth; }

    if (!n->left && !n->right) {
        if (s->shortest_path == -1 || depth < s->shortest_path) s->shortest_path = depth;
        if (s->height == -1        || depth > s->height)        s->height = depth;
    }

    /* check red links property */
    if (rbn_is_red(n) && (rbn_is_red(n->left) || rbn_is_red(n->right))) {
        log_error("rb invariant violated: subsequent red nodes");
        return -1;
    }

    /* check for 4-nodes */
    if (rbn_is_red(n->left) && rbn_is_red(n->right)) {
        log_error("rb invariant violated: 4-node in a 2-3 tree");
        return -2;
    }

    /* check black height propery */
    if (!n->left || !n->right) {
        if (s->black_height == -1) {
            s->black_height = black_depth;
        } else {
            if (s->black_height != black_depth) {
                log_error("rb invariant violated: inconsistent black height");
                return -3;
            }
        }
    }

    /* process children */
    int rc;

    rc = rbn_analyze(n->left, depth, black_depth, s);
    if (rc < 0) return rc;

    rc = rbn_analyze(n->right, depth, black_depth, s);
    if (rc < 0) return rc;

    return 0;
}

int rb_analyze(const bst *T, struct rb_stats *s)
{
    check(T && T->key_type, "no key type");

    s->height = -1;
    s->shortest_path = -1;
    s->black_height = -1;
    s->black_nodes = 0;
    s->red_nodes = 0;

    int rc = rbn_analyze(T->root, 0, 0, s);
    check((int)T->count == s->black_nodes + s->red_nodes,
            "count and actual number of nodes differ");

    return rc;
error:
    return -1;
}

int rb_invariant(const bst *T)
{
    struct rb_stats s;
    return rb_analyze(T, &s);
}
