#include <assert.h>

#include "bst.h"

static inline
btn *rb_node_rotate_left(btn *n)
{
    assert(n->right);
    btn *r = n->right;
    n->right = r->left;
    r->left = n;
    rbtn_set_color(r, rbtn_color(n));
    rbtn_set_color(n, RED);
    return r;
}

static inline
btn *rb_node_rotate_right(btn *n)
{
    assert(n->left);
    btn *l = n->left;
    n->left = l->right;
    l->right = n;
    rbtn_set_color(l, rbtn_color(n));
    rbtn_set_color(n, RED);
    return l;
}

static inline
void rb_node_color_flip(btn *n)
{
    assert(n && n->left && n->right);
    rbtn_set_color(n, !rbtn_color(n));
    rbtn_set_color(n->left, !rbtn_color(n->left));
    rbtn_set_color(n->right, !rbtn_color(n->right));
}

#define rb_node_is_red(n) ((n) && (n)->flags.rb.color == RED)

bst_node *rb_node_insert(bt *T, btn *n, const void *k)
{
    if (n == NULL) {
        btn *nnew = btn_new(T);
        btn_set_key(T, nnew, k);
        rbtn_set_color(nnew, RED);
        return nnew;
    }

    if (is_red(n->left) && is_red(n->right)) color_flip(n);

    int comp = t_compare(T->key_type, k, btn_key(T, n));
    /* if (comp == 0)      return n; */
    if (comp < 0)       n->left = insert(T, n->left, k);
    else if (comp > 0)  n->right = insert(T, n->right, k);

    if (is_red(n->right) && !is_red(n->left)) n = rotate_left(n);
    if (is_red(n->left) && n->left && is_red(n->left->left)) n = rotate_right(n);

    return n;
}

void llrb_insert(bt *T, const void *k)
{
    T->root = insert(T, T->root, k);
}
