#include <assert.h>

#include "debug.h"
#include "rbt.h"

/* Walk through all the nodes of the tree in ascending order. If at any point
 * the callback returns a non-zero integer, abort and return it. */
static int _rbt_traverse_node(_rbt_node *n,
                              int (*f)(_rbt_node *n, void *p),
                              void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = _rbt_traverse_node(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = _rbt_traverse_node(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int _rbt_invariant_node(_rbt_node *n, void *black_count)
{
    // Check for adjacent red nodes.
    if (n->color == RED) {
        if ((n->left && n->left->color == RED) || (n->right && n->right->color == RED)) {
            debug("Invariant violated: Two adjacent red nodes.")
            return -1;
        }
    }
    if (!n->left && !n->right) {
        // We are a leaf node. Count black nodes on the path to the root.
        int count = 0;
        while (n != NULL) {
            if (n->color == BLACK) ++count;
            n = n->parent;
        }
        if (*(int*)black_count == -1) {
            *(int*)black_count = count;
        } else if (count != *(int*)black_count) {
            debug("Invariant violated: Unequal numbers of black nodes per path.");
            return -2;
        }
    }

    return 0;
}

int _rbt_invariant(const _rbt *T)
{
    int black_count = -1;
    int rc = _rbt_traverse_node(T->root, _rbt_invariant_node, &black_count);
    return rc;
}

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
        if (n->data && T) {
            TypeInterface_destroy(T->element_type, n->data);
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
        TypeInterface_destroy(T->element_type, n->data);
    } else {
        n->data = TypeInterface_allocate(T->element_type, 1);
        check_alloc(n->data);
    }

    TypeInterface_copy(T->element_type, n->data, value);

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
        debug("parent = %d, old_child = %d, new_child = %d",
                *(int*)parent->data,
                *(int*)old_child->data, *(int*)new_child->data);
    } else if (old_child->data) {
        debug("parent = NULL, old_child = %d, new_child = %d",
                *(int*)old_child->data, *(int*)new_child->data);
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
    if (n->data) debug("n = %d", *(int*)n->data);
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
    if (n->data) debug("n = %d", *(int*)n->data);
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

int _rbt_init(_rbt *T, TypeInterface *element_type)
{
    check_ptr(T);
    check_ptr(element_type);
    check(element_type->compare, "No comparison function.");

    T->root = NULL;
    T->size = 0;
    T->element_type = element_type;

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
    debug("n = %d", *(int*)n->data);
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
    return n->color == BLACK
           && n->left && n->left->color == RED
           && n->right && n->right->color == RED;
}

int _rbt_node_insert(_rbt *T, _rbt_node *n, const void *value)
{
    debug("n = %d, value = %d", *(int*)n->data, *(int*)value);
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    assert(n->parent || n == T->root);

    if (_rbt_node_is_four_node(n)) {
        check(!_rbt_node_color_red(T, n), "_rbt_node_color_red failed.");
    }

    int comp = TypeInterface_compare(T->element_type, value, n->data);
    debug("comp = %d", comp);

    if (comp < 0) {
        if (n->left) {
            check(!_rbt_node_insert(T, n->left, value), "_rbt_node_insert failed.");
        } else {
            if (n->color == BLACK) {
                debug("n is BLACK");
                check(!_rbt_node_new(&n->left), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->left, value), "_rbt_node_set failed.");
                n->left->parent = n;
                n->left->color = RED;
                ++T->size;
            } else {
                debug("n is RED");
                assert(!n->right);
                if (n == n->parent->left) {
                    debug("n is a left child");
                    check(!_rbt_node_rotate_right(T, n->parent, &n),
                          "_rbt_node_rotate_right failed.");
                    check(_rbt_node_new(&n->left), "_rbt_node_new failed.");
                    check(_rbt_node_set(T, n->left, value), "_rbt_node_set failed.");
                    n->left->parent = n;
                    n->color = BLACK;
                    n->left->color = RED;
                    n->right->color = RED;
                    ++T->size;
                } else if (n == n->parent->right) {
                    debug("n is a right child");
                    check(!_rbt_node_new(&n->parent->left), "_rbt_node_new failed.");
                    check(!_rbt_node_set(T, n->parent->left, n->parent->data),
                          "_rbt_node_set failed.");
                    check(!_rbt_node_set(T, n->parent, value), "_rbt_node_set failed.");
                    n->parent->left->parent = n->parent;
                    n->parent->left->color = RED;
                    ++T->size;
                } else {
                    sentinel("n has no parent");
                }
            }
        }
    } else if (comp > 0) {
        if (n->right) {
            check(!_rbt_node_insert(T, n->right, value), "_rbt_node_insert failed.");
        } else {
            debug("no right child");
            if (n->color == BLACK) {
                debug("n is BLACK");
                check(!_rbt_node_new(&n->right), "_rbt_node_new failed.");
                check(!_rbt_node_set(T, n->right, value), "_rbt_node_set failed.");
                n->right->parent = n;
                n->right->color = RED;
                ++T->size;
            } else {
                debug("n is RED");
                assert(!n->right);
                if (n == n->parent->right) {
                    debug("n is a right child");
                    check(!_rbt_node_rotate_left(T, n->parent, &n),
                          "_rbt_node_rotate_left failed.");
                    check(_rbt_node_new(&n->right), "_rbt_node_new failed.");
                    check(_rbt_node_set(T, n->right, value), "_rbt_node_set failed.");
                    n->right->parent = n;
                    n->color = BLACK;
                    n->right->color = RED;
                    n->left->color = RED;
                    ++T->size;
                } else if (n == n->parent->left) {
                    debug("n is a left child");
                    check(!_rbt_node_new(&n->parent->right), "_rbt_node_new failed.");
                    check(!_rbt_node_set(T, n->parent->right, n->parent->data),
                          "_rbt_node_set failed.");
                    check(!_rbt_node_set(T, n->parent, value), "_rbt_node_set failed.");
                    n->parent->right->parent = n->parent;
                    n->parent->right->color = RED;
                    ++T->size;
                } else {
                    sentinel("n has no parent");
                }
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
    debug("value = %d", *(int*)value);
    check_ptr(T);
    check_ptr(value);
    assert (!_rbt_invariant(T));

    if (!T->root) {
        check(!_rbt_node_new(&T->root), "_rbt_node_new failed.");
        check(!_rbt_node_set(T, T->root, value), "_rbt_node_set failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        check(!_rbt_node_insert(T, T->root, value), "_rbt_node_insert failed.");
    }

    assert (!_rbt_invariant(T));
    return 0;
error:
    return -1;
}
