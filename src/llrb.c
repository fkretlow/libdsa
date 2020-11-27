#include <assert.h>
#include "bst.h"

#define is_red(n) ((n) && (n)->flags.rb.color == RED)

static inline
bst_node *rotate_left(bst_node *n)
{
    assert(n->right);
    bst_node *r = n->right;
    n->right = r->left;
    r->left = n;
    r->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    return r;
}

static inline
bst_node *rotate_right(bst_node *n)
{
    assert(n->left);
    bst_node *l = n->left;
    n->left = l->right;
    l->right = n;
    l->flags.rb.color = n->flags.rb.color;
    n->flags.rb.color = RED;
    return l;
}

static inline
void color_flip(bst_node *n)
{
    assert(n && n->left && n->right);
    n->flags.rb.color        = !n->flags.rb.color;
    n->left->flags.rb.color  = !n->left->flags.rb.color;
    n->right->flags.rb.color = !n->right->flags.rb.color;
}

bst_node *rb_node_insert(bst *T, bst_node *n, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    if (!n) {
        ++T->count;
        return bst_node_new(T, k, v);
    }

    int cmp = t_compare(T->key_type, k, bst_node_key(T, n));

    if (cmp < 0) {
        n->left = rb_node_insert(T, n->left, k, v);
    } else if (cmp > 0) {
        n->right = rb_node_insert(T, n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bst_node_set_value(T, n, v);
    }

    if (is_red(n->right) && !is_red(n->left))
        n = rotate_left(n);
    if (is_red(n->left) && is_red(n->left->left))
        n = rotate_right(n);
    if (is_red(n->left) && is_red(n->right))
        color_flip(n);

    return n;
}
