#include <assert.h>

#include "debug.h"
#include "rbt.h"

/* Walk through all the nodes of the tree in ascending order. If at any point
 * the callback returns a non-zero integer, abort and return it. The parameter p
 * is passed to the callback. */
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
    if (!n->left || !n->right) {
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

/* Replace the child in a node after the child was rotated. */
static inline
void _rbt_node_replace_child(_rbt *T,
                             _rbt_node *parent,
                             _rbt_node *old_child,
                             _rbt_node *new_child)
{
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
    /* if (n->data) debug("n = %d", *(int*)n->data); */
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
    /* if (n->data) debug("n = %d", *(int*)n->data); */
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

static inline void _rbt_node_clear(_rbt *T, _rbt_node *n)
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
    T->root = NULL;
    T->size = 0;
}

int _rbt_node_move_up_four_node(_rbt *T, _rbt_node *n)
{
    /* debug("n = %d", *(int*)n->data); */
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
    /* debug("n = %d, value = %d", *(int*)n->data, *(int*)value); */
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    assert((n->parent && (n == n->parent->left || n == n->parent->right))
           || n == T->root);

    int rc;

    /* On the path down to the insertion position we need to make sure that
     * every node we walk through has space for the insertion. For lack of
     * better words we use the terminology of a 2-3-4 tree (an isometry of a
     * red-black-tree), where a four-node is a node with three values (a black
     * one in the middle and a red one on either side) and four children.
     * If the group of nodes with the current node n as its root is such a
     * four-node, we can push n up into the group of nodes above, because we
     * know that our direct ancestor group is not a four-node, because we have
     * just done the same thing to it that we are about to do here. */
    if (_rbt_node_is_four_node(n)) {
        rc = _rbt_node_move_up_four_node(T, n);
        check(rc == 0, "_rbt_node_move_up_four_node failed.");
    }

    int comp = TypeInterface_compare(T->element_type, value, n->data);

    if (comp < 0) {
        if (n->left) {
            return _rbt_node_insert(T, n->left, value);
        } else {
            if (n->color == BLACK) {
                /* Case 1 (black-left): The parent is black so we can just insert a
                 * left child. */
                /* debug("Insert case 1: black-left") */
                rc = _rbt_node_new(&n->left);
                check(rc == 0, "_rbt_node_new failed.");
                n->left->parent = n;
                n->left->color = RED;

                rc = _rbt_node_set(T, n->left, value);
                check(rc == 0, "_rbt_node_set failed.");

                ++T->size;

            } else {
                /* The parent is red and we are at a leaf position. The parent
                 * cannot have another child, because that child would be
                 * black, which would violate the same-number-of-black-nodes
                 * property. */
                assert(!n->right);
                if (n == n->parent->left) {
                    /* Case 2 (red-left-left): The parent is a red left child,
                     * and we need to insert a left child. We can't just insert
                     * because the new child would be black and that would
                     * violate the same-number-of-black-nodes property. But we
                     * know that our grandfather is not a four-node so it can't
                     * have another red child. That means we can rotate it.
                     * Then our parent becomes a black node and we insert the
                     * value into a left, red child. */
                    /* debug("Insert case 2: red-left-left") */
                    rc = _rbt_node_rotate_right(T, n->parent, &n);
                    check(rc == 0, "_rbt_node_rotate_right failed.");
                    n->color = BLACK;
                    n->right->color = RED;

                    rc = _rbt_node_new(&n->left);
                    check(rc == 0, "_rbt_node_new failed.");
                    n->left->parent = n;
                    n->left->color = RED;

                    rc = _rbt_node_set(T, n->left, value);
                    check(rc == 0, "_rbt_node_set failed.");

                    ++T->size;

                } else if (n == n->parent->right) {
                    /* Case 3 (red-right-left): The parent is a red right
                     * child, and we need to insert a left child. Again, we
                     * can't just insert for the same reason as above. In this
                     * case we need to do two rotations successively after
                     * inserting the new value: First, rotate the parent right,
                     * then the grandparent left. In order to save calls, we
                     * cheat and get the same effect by adding a left child to
                     * the grandfather, which can't have another child (see
                     * above), set it to the value of the grandfather, and set
                     * the the grandfather to the new value. */
                    /* debug("Insert case 3: red-right-left") */
                    rc = _rbt_node_new(&n->parent->left);
                    check(rc == 0, "_rbt_node_new failed.");
                    n->parent->left->parent = n->parent;
                    n->parent->left->color = RED;

                    rc = _rbt_node_set(T, n->parent->left, n->parent->data);
                    check(rc == 0, "_rbt_node_set failed.");

                    rc = _rbt_node_set(T, n->parent, value);
                    check(rc == 0, "_rbt_node_set failed.");

                    ++T->size;
                }
            }
        }
    } else if (comp > 0) {
        if (n->right) {
            return _rbt_node_insert(T, n->right, value);

        } else {
            if (n->color == BLACK) {
                /* Case 4 (black-right): The parent is black so we can just
                 * insert a right child. */
                /* debug("Insert case 4: black-right"); */
                rc = _rbt_node_new(&n->right);
                check(rc == 0, "_rbt_node_new failed.");
                n->right->parent = n;
                n->right->color = RED;

                rc = _rbt_node_set(T, n->right, value);
                check(rc == 0, "_rbt_node_set failed.");

                ++T->size;

            } else {
                /* The parent is red and we are at a leaf position. The parent
                 * cannot have another child, because that child would be
                 * black, which would violate the same-number-of-black-nodes
                 * property. */
                assert(!n->right);
                if (n == n->parent->right) {
                    /* Case 5 (red-right-right): Same procedure as in case 2
                     * above with opposite directions */
                    /* debug("Insert case 5: red-right-right"); */
                    rc = _rbt_node_rotate_left(T, n->parent, &n);
                    check(rc == 0, "_rbt_node_rotate_left failed.");
                    n->color = BLACK;
                    n->left->color = RED;

                    rc = _rbt_node_new(&n->right);
                    check(rc == 0, "_rbt_node_new failed.");
                    n->right->parent = n;
                    n->right->color = RED;

                    rc = _rbt_node_set(T, n->right, value);
                    check(rc == 0, "_rbt_node_set failed.");

                    ++T->size;

                } else if (n == n->parent->left) {
                    /* Case 6 (red-left-right): Same procedure as in case 3
                     * above with opposite directions. */
                    /* debug("Insert case 6: red-left-right"); */
                    rc = _rbt_node_new(&n->parent->right);
                    check(rc == 0, "_rbt_node_new failed.");
                    n->parent->right->parent = n->parent;
                    n->parent->right->color = RED;

                    rc = _rbt_node_set(T, n->parent->right, n->parent->data);
                    check(rc == 0, "_rbt_node_set failed.");

                    rc = _rbt_node_set(T, n->parent, value);
                    check(rc == 0, "_rbt_node_set failed.");

                    ++T->size;
                }
            }
        }
    } else {
        /* debug("Insert: value is already there."); */
        return 1;
    }

    return 0;
error:
    return -1;
}

int _rbt_insert(_rbt *T, const void *value)
{
    /* debug("value = %d", *(int*)value); */
    check_ptr(T);
    check_ptr(value);
    assert (!_rbt_invariant(T));

    int rc = 0;

    if (!T->root) {
        check(!_rbt_node_new(&T->root), "_rbt_node_new failed.");
        check(!_rbt_node_set(T, T->root, value), "_rbt_node_set failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        rc = _rbt_node_insert(T, T->root, value);
        check(rc >= 0, "_rbt_node_insert failed.");
    }

    assert (!_rbt_invariant(T));
    return rc;
error:
    return -1;
}

int _rbt_has(const _rbt *T, const void *value)
{
    check_ptr(T);
    check_ptr(value);

    _rbt_node *n = T->root;
    int comp;

    while (n) {
        comp = TypeInterface_compare(T->element_type, value, n->data);
        if (comp > 0) {
            n = n->right;
        } else if (comp < 0) {
            n = n->left;
        } else {
            return 1;
        }
    }

    return 0;
error:
    return -1;
}
