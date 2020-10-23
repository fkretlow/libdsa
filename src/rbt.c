#include <assert.h>

#include "debug.h"
#include "rbt.h"

/* static inline */
int __rbt_node_new(__rbt_node **node_out)
{
    __rbt_node *n = calloc(1, sizeof(*n));
    check_alloc(n);
    *node_out = n;
    return 0;
error:
    return -1;
}

/* static inline */
void __rbt_node_delete(const __rbt *t, __rbt_node *n)
{
    if (n) {
        if (n->data && t && t->destroy) {
            t->destroy(n->data);
        }
    }
    free(n->data);
    free(n);
}

/* static */
int __rbt_node_set(const __rbt *t, __rbt_node *n, const void *value)
{
    check_ptr(t);
    check_ptr(n);
    check_ptr(value);

    if (n->data) {
        if (t->destroy) {
            t->destroy(n->data);
        }
    } else {
        n->data = malloc(t->element_size);
        check_alloc(n->data);
    }
    memmove(n->data, value, t->element_size);

    return 0;
error:
    return -1;
}

/* static inline */
void __rbt_node_replace_child(__rbt_node *n,
                              __rbt_node *old,
                              __rbt_node *succ)
{
    assert(old == n->left || old == n->right);
    if (old == n->left) { n->left  = succ; }
    else                { n->right = succ; }
    succ->parent = n;
}

/* static inline */
int __rbt_node_rotate_left(__rbt_node *n, __rbt_node **node_out)
{
    check_ptr(n);
    assert(n->right && n->right->left);

    __rbt_node *p  = n->parent;
    __rbt_node *r  = n->right;
    __rbt_node *rl = r->left;

    r->left = n, n->parent = r;
    n->right = rl, rl->parent = n;
    if (p) __rbt_node_replace_child(p, n, r);

    if (node_out) *node_out = r;
    return 0;
error:
    return -1;
}

/* static inline */
int __rbt_node_rotate_right(__rbt_node *n, __rbt_node **node_out)
{
    check_ptr(n);
    assert(n->left && n->left->right);

    __rbt_node *p  = n->parent;
    __rbt_node *l  = n->left;
    __rbt_node *lr = l->right;

    l->right = n, n->parent = l;
    n->left = lr, lr->parent = n;
    if (p) __rbt_node_replace_child(p, n, l);

    if (node_out) *node_out = l;
    return 0;
error:
    return -1;
}
