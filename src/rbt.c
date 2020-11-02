#include <assert.h>

#include "debug.h"
#include "rbt.h"

/* Walk through all the nodes of the tree in ascending order. If at any point the
 * callback returns a non-zero integer, abort and return it. The parameter p is passed
 * to the callback. */
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

int _rbt_node_invariant(_rbt_node *n, void *black_count)
{
    if (n->color == RED) {
        /* Check for adjacent red nodes. */
        if ((n->left && n->left->color == RED) || (n->right && n->right->color == RED)) {
            debug("Invariant violated: Two adjacent red nodes.")
            return -1;
        }
    }
    if (!n->left || !n->right) {
        /* This is a leaf node. Count black nodes on the path to the root. */
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
    int rc = _rbt_traverse_node(T->root, _rbt_node_invariant, &black_count);
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

/* The rotation routines don't recolor the nodes. That is done in the insertion and
 * deletion algorithms according to the way different rotations are combined. */
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

static inline int _rbt_node_is_full_group(_rbt_node *n)
{
    return n->color == BLACK
           && n->left && n->left->color == RED
           && n->right && n->right->color == RED;
}

int _rbt_node_make_space_in_group(_rbt *T, _rbt_node *n)
{
    /* debug("n = %d", *(int*)n->data); */
    /* This routine should never be called on a node that is not the root of a four-node. */
    assert(_rbt_node_is_full_group(n));

    int rc;
    _rbt_node *p = n->parent;

    if (!p) {
        /* Case 1 (root): This is the root. Breaking up the four-node is as easy as
         * painting both children black. */
        assert(n == T->root);
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == BLACK) {
        /* Case 2 (black): The parent is a black node. We simply move n up into the
         * parent group by painting it red. Then both children can become black nodes.
         * */
        n->color = RED;
        n->left->color = n->right->color = BLACK;
    }

    else if (p->color == RED) {
        /* Okay, the parent is red. Now things get a little more involved. We need
         * pointers to the grandparent and to both of n's children. */
        _rbt_node *pp = p->parent;
        _rbt_node *cl = n->left;
        _rbt_node *cr = n->right;

        if (n == p->left && p == pp->left) {
            /* Case 3 (red-left-left): Both n and its parent are left nodes. We rotate
             * the parent right and recolor. */
            rc = _rbt_node_rotate_right(T, pp, NULL);
            check(rc == 0, "_rbt_node_rotate_right failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->left && p == pp->right) {
            /* Case 4 (red-right-left): The parent is a right child and n is its left
             * child. We need to rotate the parent right, then the grandparent left, and
             * recolor. */
            rc = _rbt_node_rotate_right(T, p, NULL);
            check(rc == 0, "_rbt_node_rotate_right failed.");
            rc = _rbt_node_rotate_left(T, pp, NULL);
            check(rc == 0, "_rbt_node_rotate_left failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->right) {
            /* Case 5 (red-right-right): Same as case 2 with opposite directions. */
            rc = _rbt_node_rotate_left(T, pp, NULL);
            check(rc == 0, "_rbt_node_rotate_left failed.");
            p->color = BLACK;
            pp->color = RED;
            n->color = RED;
            cl->color = cr->color = BLACK;

        } else if (n == p->right && p == pp->left) {
            /* Case 6 (red-left-right): Same as case 4 wit opposite directions. */
            rc = _rbt_node_rotate_left(T, p, NULL);
            check(rc == 0, "_rbt_node_rotate_left failed.");
            rc = _rbt_node_rotate_right(T, pp, NULL);
            check(rc == 0, "_rbt_node_rotate_right failed.");
            p->color = pp->color = RED;
            cl->color = cr->color = BLACK;
        }
    }

    return 0;
error:
    return -1;
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

    /* On the path down to the insertion position we need to make sure that every group
     * we walk through has space for the insertion. If the group with the current node n
     * at the root is full, we can push n up into the group above, because we know that
     * our direct ancestor group is not full, because we have just done the same thing
     * to it that we are about to do here. */
    if (_rbt_node_is_full_group(n)) {
        rc = _rbt_node_make_space_in_group(T, n);
        check(rc == 0, "_rbt_node_make_space_in_group failed.");
    }

    int comp = TypeInterface_compare(T->element_type, value, n->data);

    if (comp < 0) {
        if (n->left) {
            return _rbt_node_insert(T, n->left, value);

        } else {
            if (n->color == BLACK) {
                /* Case 1 (black-left): The parent is black so we can just insert a red
                 * child. */
                /* debug("Insert case 1: black-left") */
                rc = _rbt_node_new(&n->left);
                check(rc == 0, "_rbt_node_new failed.");
                n->left->parent = n;
                n->left->color = RED;

                rc = _rbt_node_set(T, n->left, value);
                check(rc == 0, "_rbt_node_set failed.");

                ++T->size;

            } else {
                /* The parent is red and we are at a leaf position. The parent cannot
                 * have another child, because that child would be black, which would
                 * violate the same-number-of-black-nodes property. */
                assert(!n->right);
                if (n == n->parent->left) {
                    /* Case 2 (red-left-left): The parent is a red left child, and we
                     * need to insert a left child. We can't just insert because the new
                     * child would be black and that would violate the same-number-of-
                     * black-nodes property. But we know that our grandparent is not a
                     * four-node so it can't have another red child. That means we can
                     * rotate it.  Then our parent becomes a black node and we insert
                     * the value into a left, red child. */
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
                    /* Case 3 (red-right-left): The parent is a red right child, and we
                     * need to insert a left child. Again, we can't just insert for the
                     * same reason as above. In this case we need to do two rotations
                     * successively after inserting the new value: First, rotate the
                     * parent right, then the grandparent left. In order to save calls,
                     * we cheat and get the same effect by adding a left child to the
                     * grandparent, which can't have another child (see above), set it
                     * to the value of the grandparent, and set the grandparent to the
                     * new value. */
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
                /* Case 4 (black-right): Same as case 1. */
                /* debug("Insert case 4: black-right"); */
                rc = _rbt_node_new(&n->right);
                check(rc == 0, "_rbt_node_new failed.");
                n->right->parent = n;
                n->right->color = RED;

                rc = _rbt_node_set(T, n->right, value);
                check(rc == 0, "_rbt_node_set failed.");

                ++T->size;

            } else {
                /* The parent is red and we are at a leaf position. The parent cannot
                 * have another child, because that child would be black, which would
                 * violate the same-number-of-black-nodes property. */
                assert(!n->right);
                if (n == n->parent->right) {
                    /* Case 5 (red-right-right): Same procedure as in case 2 above with
                     * opposite directions */
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
                    /* Case 6 (red-left-right): Same procedure as in case 3 above with
                     * opposite directions. */
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
        rc = _rbt_node_new(&T->root);
        check(rc == 0, "_rbt_node_new failed.");
        rc = _rbt_node_set(T, T->root, value);
        check(rc == 0, "_rbt_node_set failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        rc = _rbt_node_insert(T, T->root, value);
        check(rc >= 0, "_rbt_node_insert failed.");
    }

    assert(!_rbt_invariant(T));
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

static inline int _rbt_node_is_empty_group(_rbt_node *n)
{
    return n->color == BLACK
           && (!n->left || n->left->color == BLACK)
           && (!n->right || n->right->color == BLACK);
}

int _rbt_node_fill_group(_rbt *T, _rbt_node *n)
{
    assert(!_rbt_invariant(T));
    /* debug("n = %d", *(int*)n->data); */
    /* This routine should never be called on a node with any red children. */
    assert(_rbt_node_is_empty_group(n));

    int rc = -1;
    _rbt_node *p = n->parent;
    assert(p || n == T->root);
    _rbt_node *s = NULL; /* The sibling node. */

    if (n == T->root) {
        /* debug("n is the root"); */
        if (n->left && _rbt_node_is_empty_group(n->left)
                && n->right &&  _rbt_node_is_empty_group(n->right)) {
            /* debug("transforming 3*black at the root"); */
            /* Handle the case of an empty group at the root of the tree where both
             * descendant groups are also empty. I.o.w. three black nodes at the top. */
            n->left->color = n->right->color = RED;
        }
        /* n is the root and it's an empty group. But we don't know at this point in
         * which direction we'll move. We'll need to handle the case of an empty root
         * group as a direct ancestor later. */
        assert(!_rbt_invariant(T));
        return 0;
    }

    if (n == p->right) {
        /* debug("right child"); */
        s = p->left;
        /* debug("p->color = %d, s->color = %d, n->color = %d", p->color, s->color, n->color); */

        if (p->color == BLACK) {
            /* debug("black parent"); */
            /* Assume that the direct ancestor group is not empty unless it is the root
             * of the tree... */
            assert(!_rbt_node_is_empty_group(p) || p == T->root);

            if (p == T->root && _rbt_node_is_empty_group(p)) {
                /* debug("ancestor group is empty and root"); */
                /* ... in which case we need some special shenanigans to happen:
                 * Assume that the sibling is black, and not empty because that case
                 * would have been handled above. */
                assert(s->left || s->right);

                if (_rbt_node_is_full_group(s)) {
                    /* Rotate the sibling left, then p right, and recolor. */
                    rc = _rbt_node_rotate_left(T, s, NULL);
                    check(rc == 0, "_rbt_node_rotate_left failed.");

                    rc = _rbt_node_rotate_right(T, p, NULL);
                    check(rc == 0, "_rbt_node_rotate_right failed.");

                    /* p is no longer the root of the tree. Its successor, the former
                     * right child of s, was originally red. */
                    p->parent->color = BLACK;
                    n->color = RED;

                } else { /* The sibling has one red child. */
                    if (s->left && s->left->color == BLACK) {
                        /* Make sure the sibling has an outer (i.e. left) red child. */
                        rc = _rbt_node_rotate_left(T, s, &s);
                        check(rc == 0, "_rbt_node_rotate_left failed.");

                        /* Note that s has been updated to hold the new sibling. */
                        s->color = BLACK;
                        s->left->color = RED;
                    }

                    rc = _rbt_node_rotate_right(T, p, NULL);
                    check(rc == 0, "_rbt_node_rotate_right failed.");

                    /* Now s is at the root of the tree and it's p's parent. */
                    assert(s == p->parent);
                    s->left->color = BLACK;
                    n->color = RED;
                }
                /* End of the special root case. Ooof! */
                return 0; // ???

            } else {
                /* Black parent, but it's not an empty group: The sibling is red.
                 * Prepare for the actual happening below by rotating the ancestor group
                 * so the parent node becomes red. */
                rc = _rbt_node_rotate_right(T, p, NULL);
                check(rc == 0, "_rbt_node_rotate_right failed.");
                p->parent->color = BLACK;
                p->color = RED;
            }
        } /* endif p->color == BLACK */

        /* Now we are guaranteed to have a red parent. */
        assert(!_rbt_invariant(T));
        assert(p->color == RED);
        assert(p == n->parent);
        s = p->left;

        if (_rbt_node_is_empty_group(s)) {
            /* debug("sibling is an empty group"); */
            /* The sibling is an empty group, too. We combine the red parent and both
             * the sibling and the current node into a single full group by just
             * inverting the colors. */
            p->color = BLACK;
            s->color = n->color = RED;
            assert(!_rbt_invariant(T));

        } else {
            /* debug("sibling has red children"); */
            if (!s->left || s->left->color == BLACK) {
                /* debug("sibling has no left red child"); */
                /* The sibling has no outer (left) red child. We need to rotate it
                 * inwards first. */
                rc = _rbt_node_rotate_left(T, s, &s);
                check(rc == 0, "_rbt_node_rotate_left failed.");
                assert(s == p->left);
                /* Note that s has been updated to hold the new left sibling. */
                s->color = BLACK;
                s->left->color = RED;
            }

            rc = _rbt_node_rotate_right(T, p, NULL);
            check(rc == 0, "_rbt_node_rotate_right failed.");
            p->parent->color = RED;
            p->parent->left->color = p->color = BLACK;
            n->color = RED;
            assert(!_rbt_invariant(T));
        }

    } else { /* n == p->left */
        /* debug("left child"); */
        s = p->right;
        /* debug("p->color = %d, s->color = %d, n->color = %d", p->color, s->color, n->color); */

        if (p->color == BLACK) {
            /* debug("black parent"); */
            /* Assume that the direct ancestor group is not empty unless it is the root
             * of the tree... */
            assert(!_rbt_node_is_empty_group(p) || p == T->root);

            if (p == T->root && _rbt_node_is_empty_group(p)) {
                /* debug("ancestor group is empty and root"); */
                /* ... in which case we need some special shenanigans to happen:
                 * Assume that the sibling is black, and not empty because that case
                 * would have been handled above. */
                assert(s->left || s->right);

                if (_rbt_node_is_full_group(s)) {
                    /* debug("sibling is a full group"); */

                    /* Rotate the sibling right, then p left, and recolor. */
                    rc = _rbt_node_rotate_right(T, s, NULL);
                    check(rc == 0, "_rbt_node_rotate_right failed.");

                    rc = _rbt_node_rotate_left(T, p, NULL);
                    check(rc == 0, "_rbt_node_rotate_left failed.");

                    /* p is no longer the root of the tree. Its successor, the former
                     * left child of s, was originally red. */
                    p->parent->color = BLACK;
                    n->color = RED;

                } else { /* The sibling has one red child. */
                    /* debug("sibling has a red child"); */
                    if (s->right && s->right->color == BLACK) {
                        /* debug("... the left one, need to rotate it"); */
                        /* Make sure the sibling has an outer (i.e. right) red child. */
                        rc = _rbt_node_rotate_right(T, s, &s);
                        check(rc == 0, "_rbt_node_rotate_left failed.");

                        /* Note that s has been updated to hold the new sibling. */
                        s->color = BLACK;
                        s->right->color = RED;
                    }

                    rc = _rbt_node_rotate_left(T, p, NULL);
                    check(rc == 0, "_rbt_node_rotate_left failed.");

                    /* Now s is at the root of the tree and it's p's parent. */
                    s->right->color = BLACK;
                    n->color = RED;
                }
                /* End of the special root case. */
                return 0; // ???

            } else {
                /* Black parent, but it's not an empty group: The sibling is red.
                 * Prepare for the actual happening below by rotating the ancestor group
                 * so the parent node becomes red. */
                /* debug("right sibling is red"); */
                assert(s->color == RED);
                rc = _rbt_node_rotate_left(T, p, NULL);
                check(rc == 0, "_rbt_node_rotate_right failed.");
                p->parent->color = BLACK;
                p->color = RED;
            }
        } /* endif p->color == BLACK */

        /* Now we are guaranteed to have a red parent. */
        assert(!_rbt_invariant(T));
        assert(p->color == RED);
        assert(p == n->parent);
        s = p->right;

        if (_rbt_node_is_empty_group(s)) {
            /* debug("sibling is an empty group"); */
            /* The sibling is an empty group, too. We combine the red parent and both
             * the sibling and the current node into a single full group by just
             * inverting the colors. */
            /* debug("p->color = %d, s->color = %d, n->color = %d", p->color, s->color, n->color); */
            p->color = BLACK;
            s->color = n->color = RED;
            assert(!_rbt_invariant(T));

        } else {
            /* debug("sibling has red children"); */
            if (!s->right || s->right->color == BLACK) {
                /* debug("sibling has no right red child"); */
                /* The sibling has no outer (right) red child. We need to rotate it
                 * inwards first. */
                assert(s == p->right);
                rc = _rbt_node_rotate_right(T, s, &s);
                check(rc == 0, "_rbt_node_rotate_right failed.");
                assert(s == p->right);
                /* Note that s has been updated to hold the new right sibling. */
                s->color = BLACK;
                s->right->color = RED;
            }

            rc = _rbt_node_rotate_left(T, p, NULL);
            check(rc == 0, "_rbt_node_rotate_left failed.");
            p->parent->color = RED;
            p->parent->right->color = p->color = BLACK;
            n->color = RED;
            assert(!_rbt_invariant(T));
        }
    }

    return 0;
error:
    return -1;
}

static inline _rbt_node *_rbt_node_get_smallest_child(_rbt_node *n)
{
    while (n->left) n = n->left;
    return n;
}

static int _rbt_node_remove(_rbt *T, _rbt_node *n, const void *value)
{
    /* debug("n = %d, v = %d, n->color = %d", *(int*)n->data, *(int*)value, n->color); */
    check_ptr(T);
    check_ptr(n);
    check_ptr(value);

    assert(!_rbt_invariant(T));
    assert((n->parent && (n == n->parent->left || n == n->parent->right))
           || n == T->root);

    int rc;

    /* Make sure we have leeway for the deletion: Need at least one red node in every
     * group we walk through. */
    if (_rbt_node_is_empty_group(n)) {
        rc = _rbt_node_fill_group(T, n);
        check(rc == 0, "_rbt_node_fill_group failed.");
    }
    assert(!_rbt_invariant(T));

    if (n->color == BLACK) assert(n == T->root || ((n->left && n->left->color == RED) || (n->right && n->right->color == RED)));
    if (n->color == RED) assert(n->parent->color == BLACK);
    /* assert((n->color == BLACK && (n->left->color == RED || n->right->color == RED))
           || (n->color == RED && n->parent->color == BLACK)); */

    int comp = TypeInterface_compare(T->element_type, value, n->data);

    if (comp < 0) {
        if (!n->left) {
            return 0; /* Didn't find it. */
        } else {
            return _rbt_node_remove(T, n->left, value);
        }

    } else if (comp > 0) {
        if (!n->right) {
            return 0;
        } else {
            return _rbt_node_remove(T, n->right, value);
        }

    } else { /* comp == 0 */
        if (!n->left || !n->right) {
            /* This is a leaf node. We can probably just delete. */
            if (n->color == BLACK) {
                if (n == T->root && !n->left && !n->right) {
                    /* Hurray, we're at the root and the last one. Easy. */
                    _rbt_node_delete(T, n);
                    T->root = NULL;
                    return 1;
                }

                /* We're not at the root and not red... need to rotate first. */
                assert(n->left || n->right);
                if (n->right) {
                    rc = _rbt_node_rotate_left(T, n, NULL);
                    check(rc == 0, "_rbt_node_rotate_left failed.");
                } else {
                    rc = _rbt_node_rotate_right(T, n, NULL);
                    check(rc == 0, "_rbt_node_rotate_right failed.");
                }
                n->parent->color = BLACK;
            }

            if (n == T->root) {
                T->root = NULL;
            } else if (n == n->parent->left) {
                n->parent->left = NULL;
            } else { /* n == n->parent->right */
                n->parent->right = NULL;
            }
            _rbt_node_delete(T, n);
            return 1; /* Found it. */

        } else {
            /* Oh well... replace this value with the smallest greater value. Then
             * delete the leaf node where that came from. */
            _rbt_node *succ = _rbt_node_get_smallest_child(n->right);
            check(succ && succ->data, "Failed to find a suitable successor.");

            rc = _rbt_node_set(T, n, succ->data);
            check(rc == 0, "_rbt_node_set failed.");

            return _rbt_node_remove(T, n->right, succ->data);
        }
    }
error:
    return -1;
}

int _rbt_remove(_rbt *T, const void *value)
{
    /* debug("v = %d", *(int*)value); */
    check_ptr(T);
    check_ptr(value);
    assert (!_rbt_invariant(T));

    int rc = 0;

    if (T->root) {
        assert(!_rbt_invariant(T));
        rc = _rbt_node_remove(T, T->root, value);
        check(rc >= 0, "_rbt_node_insert failed.");
    }

    assert(!_rbt_invariant(T));
    return rc;
error:
    return -1;

}
