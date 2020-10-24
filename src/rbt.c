#include <assert.h>

#include "debug.h"
#include "rbt.h"

/* static inline */
int _rbt_node_new(_rbt_node **node_out)
{
    _rbt_node *n = calloc(1, sizeof(*n));
    check_alloc(n);
    *node_out = n;
    return 0;
error:
    return -1;
}

/* static inline */
void _rbt_node_delete(const _rbt *T, _rbt_node *n)
{
    if (n) {
        if (n->data && T && T->destroy) {
            T->destroy(n->data);
        }
        free(n->data);
        free(n);
    }
}

/* static */
int _rbt_node_set(const _rbt *T, _rbt_node *n, const void *value)
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
void _rbt_node_replace_child(_rbt *T,
                              _rbt_node *parent,
                              _rbt_node *old_child,
                              _rbt_node *new_child)
{
    if (parent && parent->data) {
        debug("%s: parent = %d, old_child = %d, new_child = %d",
                __func__, *(int*)parent->data,
                *(int*)old_child->data, *(int*)new_child->data);
    } else if (old_child->data) {
        debug("%s: parent = NULL, old_child = %d, new_child = %d",
                __func__, *(int*)old_child->data, *(int*)new_child->data);
    } else {
        debug("%s", __func__);
    }

    if (!parent) {
        assert(old_child == T->root);
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
int _rbt_node_rotate_left(_rbt *T, _rbt_node *n, _rbt_node **node_out)
{
    if (n->data) debug("%s: n = %d", __func__, *(int*)n->data);
    check_ptr(n);
    assert(n->right);

    _rbt_node *p  = n->parent;
    _rbt_node *r  = n->right;
    _rbt_node *rl = r->left;

    r->left = n, n->parent = r;
    n->right = rl;
    if (rl) rl->parent = n;
    _rbt_node_replace_child(T, p, n, r);

    if (node_out) *node_out = r;
    return 0;
error:
    return -1;
}

/* static */
int _rbt_node_rotate_right(_rbt *T, _rbt_node *n, _rbt_node **node_out)
{
    if (n->data) debug("%s: n = %d", __func__, *(int*)n->data);
    check_ptr(n);
    assert(n->left);

    _rbt_node *p  = n->parent;
    _rbt_node *l  = n->left;
    _rbt_node *lr = l->right;

    l->right = n, n->parent = l;
    n->left = lr;
    if (lr) lr->parent = n;
    _rbt_node_replace_child(T, p, n, l);

    if (node_out) *node_out = l;
    return 0;
error:
    return -1;
}

int _rbt_init(_rbt *T,
               const size_t element_size,
               _compare_f compare,
               _destroy_f destroy)
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

void _rbt_node_clear(_rbt *T, _rbt_node *n)
{
    if (n) {
        if (n->left) _rbt_node_clear(T, n->left);
        if (n->right) _rbt_node_clear(T, n->right);
        _rbt_node_delete(T, n);
    }
}

void _rbt_clear(_rbt *T)
{
    if (T) _rbt_node_clear(T, T->root);
    T->size = 0;
}

int _rbt_node_color_red(_rbt *T, _rbt_node *n)
{
    debug("_rbt_node_color_red: n = %d", *(int*)n->data);
    assert(n->color == BLACK && n->left->color == RED && n->right->color == RED);

    _rbt_node *p = n->parent;

    if (!p) {
        assert(n == T->root);
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == BLACK) {
        n->color = RED;
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == RED) {
        _rbt_node *pp = p->parent;
        _rbt_node *cl = n->left;
        _rbt_node *cr = n->right;

        if (n == p->left && p == pp->left) {
            check(!_rbt_node_rotate_right(T, pp, NULL),
                  "_rbt_node_rotate_right failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->left && p == pp->right) {
            check(!_rbt_node_rotate_right(T, p, NULL),
                  "_rbt_node_rotate_right failed.");
            check(!_rbt_node_rotate_left(T, pp, NULL),
                  "_rbt_node_rotate_left failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->right) {
            check(!_rbt_node_rotate_left(T, pp, NULL),
                  "_rbt_node_rotate_left failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->left) {
            check(!_rbt_node_rotate_left(T, p, NULL),
                  "_rbt_node_rotate_left failed.");
            check(!_rbt_node_rotate_right(T, pp, NULL),
                  "_rbt_node_rotate_right failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;
        }
    }

    return 0;
error:
    return -1;
}

static inline int _rbt_node_is_four_node(_rbt_node *n)
{
    return n->color == BLACK && n->left && n->left->color == RED && n->right && n->right->color == RED;
}

int _rbt_node_insert(_rbt *T, _rbt_node *n, const void *value)
{
    debug("_rbt_node_insert: n = %d, value = %d", *(int*)n->data, *(int*)value);
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    if (_rbt_node_is_four_node(n)) {
        check(!_rbt_node_color_red(T, n), "_rbt_node_color_red failed.");
    }

    int comp = T->compare(value, n->data);
    debug("_rbt_node_insert: comp = %d", comp);

    if (comp < 0) {
        if (n->left) {
            debug("_rbt_node_insert: handing off to left child node");
            check(!_rbt_node_insert(T, n->left, value), "_rbt_node_insert failed.");
        } else {
            if (n->color == BLACK) {
                check(!_rbt_node_new(&n->left), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->left, value), "_rbt_node_set failed.");
                n->left->parent = n;
                n->left->color = RED;
                ++T->size;
            } else {
                assert(!n->right);
                check(!_rbt_node_rotate_right(T, n->parent, NULL),
                        "_rbt_node_rotate_right failed.");
                n->color = BLACK;
                n->right->color = RED;
                check(!_rbt_node_new(&n->left), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->left, value), "_rbt_node_set failed.");
                n->left->parent = n;
                n->left->color = RED;
                ++T->size;
            }
        }
    } else if (comp > 0) {
        if (n->right) {
            debug("_rbt_node_insert: handing off to right child node");
            check(!_rbt_node_insert(T, n->right, value), "_rbt_node_insert failed.");
        } else {
            debug("_rbt_node_insert: no right child");
            if (n->color == BLACK) {
                debug("%s: n is BLACK", __func__);
                check(!_rbt_node_new(&n->right), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->right, value), "_rbt_node_set failed.");
                n->right->parent = n;
                n->right->color = RED;
                ++T->size;
            } else {
                debug("%s: n is RED", __func__);
                assert(!n->left);
                check(!_rbt_node_rotate_left(T, n->parent, NULL),
                        "_rbt_node_rotate_left failed.");
                n->color = BLACK;
                n->left->color = RED;
                check(!_rbt_node_new(&n->right), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->right, value), "_rbt_node_set failed.");
                n->right->parent = n;
                n->right->color = RED;
                ++T->size;
            }
        }
    } else {
        debug("inserting value that is already there");
    }

    return 0;
error:
    return -1;
}

int _rbt_insert(_rbt *T, const void *value)
{
    debug("_rbt_insert: value = %d", *(int*)value);
    check_ptr(T);
    check_ptr(value);

    if (!T->root) {
        check(!_rbt_node_new(&T->root), "_rbt_node_new failed.");
        check(!_rbt_node_set(T, T->root, value), "_rbt_node_set failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        check(!_rbt_node_insert(T, T->root, value), "_rbt_node_insert failed.");
    }

    return 0;
error:
    return -1;
}
