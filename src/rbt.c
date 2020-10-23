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
void __rbt_node_delete(const __rbt *T, __rbt_node *n)
{
    if (n) {
        if (n->data && T && T->destroy) {
            T->destroy(n->data);
        }
    }
    free(n->data);
    free(n);
}

/* static */
int __rbt_node_set(const __rbt *T, __rbt_node *n, const void *value)
{
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    if (n->data) {
        if (T->destroy) {
            T->destroy(n->data);
        }
    } else {
        n->data = malloc(T->element_size);
        check_alloc(n->data);
    }
    memmove(n->data, value, T->element_size);

    return 0;
error:
    return -1;
}

static inline
void __rbt_node_replace_child(__rbt *T,
                              __rbt_node *parent,
                              __rbt_node *old_child,
                              __rbt_node *new_child)
{
    if (parent == T->root) {
        assert(T->root == old_child);
        T->root = new_child;
        new_child->parent = NULL;
    } else {
        assert(old_child == parent->left || old_child == parent->right);
        if (old_child == parent->left) { parent->left  = new_child; }
        else                           { parent->right = new_child; }
        new_child->parent = parent;
    }
}

/* static */
int __rbt_node_rotate_left(__rbt *T, __rbt_node *n, __rbt_node **node_out)
{
    check_ptr(n);
    assert(n->right && n->right->left);

    __rbt_node *p  = n->parent;
    __rbt_node *r  = n->right;
    __rbt_node *rl = r->left;

    r->left = n, n->parent = r;
    n->right = rl, rl->parent = n;
    if (p) __rbt_node_replace_child(T, p, n, r);

    if (node_out) *node_out = r;
    return 0;
error:
    return -1;
}

/* static */
int __rbt_node_rotate_right(__rbt *T, __rbt_node *n, __rbt_node **node_out)
{
    check_ptr(n);
    assert(n->left && n->left->right);

    __rbt_node *p  = n->parent;
    __rbt_node *l  = n->left;
    __rbt_node *lr = l->right;

    l->right = n, n->parent = l;
    n->left = lr, lr->parent = n;
    if (p) __rbt_node_replace_child(T, p, n, l);

    if (node_out) *node_out = l;
    return 0;
error:
    return -1;
}

int __rbt_init(__rbt *T,
               const size_t element_size,
               __compare_f compare,
               __destroy_f destroy)
{
    check_ptr(T);

    T->root = NULL;
    T->element_size = element_size;
    T->size = 0;
    T->compare = compare;
    T->destroy = destroy;

    return 0;
error:
    return -1;
}

int __rbt_node_color_red(__rbt *T, __rbt_node *n)
{
    assert(n->color == BLACK && n->left->color == RED && n->right->color == RED);

    __rbt_node *p = n->parent;

    if (!p) {
        assert(n == T->root);
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == BLACK) {
        n->color = RED;
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == RED) {
        __rbt_node *pp = p->parent;
        __rbt_node *cl = n->left;
        __rbt_node *cr = n->right;

        if (n == p->left && p == pp->left) {
            check(!__rbt_node_rotate_right(T, pp, NULL),
                  "__rbt_node_rotate_right failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->left && p == pp->right) {
            check(!__rbt_node_rotate_right(T, p, NULL),
                  "__rbt_node_rotate_right failed.");
            check(!__rbt_node_rotate_left(T, pp, NULL),
                  "__rbt_node_rotate_left failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->right) {
            check(!__rbt_node_rotate_left(T, pp, NULL),
                  "__rbt_node_rotate_left failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->left) {
            check(!__rbt_node_rotate_left(T, p, NULL),
                  "__rbt_node_rotate_left failed.");
            check(!__rbt_node_rotate_right(T, pp, NULL),
                  "__rbt_node_rotate_right failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;
        }
    }

    return 0;
error:
    return -1;
}

int __rbt_node_insert(__rbt *T, __rbt_node *n, const void *value)
{
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    if (n->color == BLACK
            && n->left && n->left->color == RED
            && n->right && n->right->color == RED) {
        check(!__rbt_node_color_red(T, n), "__rbt_node_color_red failed.");
    }

    int comp = T->compare(value, n->data);
    if (comp < 0) {
        // TODO...
    }

    return 0;
error:
    return -1;
}

int __rbt_insert(__rbt *T, const void *value)
{
    check_ptr(T);
    check_ptr(value);

    if (!T->root) {
        check(!__rbt_node_new(&T->root), "__rbt_node_new failed.");
        check(!__rbt_node_set(T, T->root, value), "__rbt_node_set failed.");
        T->root->color = BLACK;
    } else {
        check(!__rbt_node_insert(T, T->root, value), "__rbt_node_insert failed.");
    }

    return 0;
error:
    return -1;
}
