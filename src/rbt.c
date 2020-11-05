#include <assert.h>

#include "debug.h"
#include "rbt.h"

int _total = 0;
int _needed = 0;

/* Walk through all the nodes of the tree in ascending order. If at any point the
 * callback returns a non-zero integer, abort and return it. The parameter p is passed
 * to the callback. */
static int _rbt_node_traverse(_rbt_node *n,
                              int (*f)(_rbt_node *n, void *p),
                              void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = _rbt_node_traverse(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = _rbt_node_traverse(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int _rbt_traverse(const _rbt *T, int (*f)(_rbt_node *n, void *p), void *p) {
    if (T && T->root) {
        return _rbt_node_traverse(T->root, f, p);
    }
    return 0;
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
    int rc = _rbt_node_traverse(T->root, _rbt_node_invariant, &black_count);
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

static inline unsigned short _rbt_group_weight(_rbt_node *n)
{
    assert(n->color == BLACK);
    unsigned short w = 0;
    if (n->left && n->left->color == RED) ++w;
    if (n->right && n->right->color == RED) ++w;
    return w;
}

static inline int _rbt_node_is_full_group(_rbt_node *n)
{
    return n->color == BLACK
           && n->left && n->left->color == RED
           && n->right && n->right->color == RED;
}

static void _rbt_group_decrease_weight(_rbt *T, _rbt_node *n)
{
    _rbt_node *p = n->parent;
    _rbt_node *pp = p ? p->parent : NULL;
    _rbt_node *l = n->left;
    _rbt_node *r = n->right;

    assert(_rbt_group_weight(n) == 2);

    /* Case 1: n is the root of the tree. */
    if (n == T->root) {
        /* debug("Case 1: n is the root"); */
        l->color = r->color = BLACK;
    }

    /* Case 2: p is black. */
    else if (p->color == BLACK) {
        /* debug("Case 2: p is black"); */
        n->color = RED;
        l->color = r->color = BLACK;
    }

    /* Case 3: p is red. */
    else if (p->color == RED) {
        /* Case 3.1: pp has 1 red child (p) */
        _rbt_node *ps = p == pp->left ? pp->right : pp->left;
        assert(ps);
        if (ps->color == BLACK) {
            /* Case 3.1a: left-left or right-right ancestor chain. */
            if (n == pp->right->right || n == pp->left->left) {
                /* debug("Case 3.1a straight ancestor chain"); */
                if (n == pp->right->right) {
                    _rbt_node_rotate_left(T, pp, NULL);
                } else {
                    _rbt_node_rotate_right(T, pp, NULL);
                }
                pp->color = RED;
                p->color = BLACK;
                n->color = RED;
                l->color = r->color = BLACK;
            }

            /* Case 3.1b: right-left or left-right ancestor chain. */
            else {
                /* debug("Case 3.1b: right-left or left-right ancestor chain"); */
                if (n == pp->right->left) {
                    _rbt_node_rotate_right(T, p, NULL);
                    _rbt_node_rotate_left(T, pp, NULL);
                } else {
                    _rbt_node_rotate_left(T, p, NULL);
                    _rbt_node_rotate_right(T, pp, NULL);
                }
                pp->color = RED;
                l->color = r->color = BLACK;
            }
        }

        /* Case 3.2: pp has 2 red children */
        else {
            /* debug("Case 3.2: pp is full."); */
            _rbt_group_decrease_weight(T, pp);
            /* Things may have changed. Go again. */
            _rbt_group_decrease_weight(T, n);
        }
    }
}

static int _rbt_node_insert(_rbt *T, _rbt_node *n, const void *v)
{
    /* debug("n = %d, v = %d", *(int*)n->data, *(int*)v); */
    int comp = TypeInterface_compare(T->element_type, v, n->data);
    if (comp == 0) {
        return 1;
    } else if (comp < 0 && n->left) {
        return _rbt_node_insert(T, n->left, v);
    } else if (comp > 0 && n->right) {
        return _rbt_node_insert(T, n->right, v);
    }

    /* Case 1: n is black */
    if (n->color == BLACK) {
        if (comp < 0) {
            /* debug("Case 1 left"); */
            _rbt_node_new(&n->left);
            _rbt_node_set(T, n->left, v);
            n->left->parent = n;
            n->left->color = RED;
            ++T->size;
        } else { /* comp > 0 */
            /* debug("Case 1 right"); */
            _rbt_node_new(&n->right);
            _rbt_node_set(T, n->right, v);
            n->right->parent = n;
            n->right->color = RED;
            ++T->size;
        }
    }

    /* Case 2: n is red */
    else {
        _rbt_node *p = n->parent;

        /* Case 2.1: p has 2 red children */
        if (_rbt_group_weight(p) == 2) {
            /* debug("Case 2.1 p is full"); */
            _rbt_group_decrease_weight(T, p);
            /* Now n may be elsewhere. Go again. */
            return _rbt_node_insert(T, n, v);
        }

        /* Case 2.2: p has 1 red child (n) */
        else { /* weight(p) = 0 is handled in case 1 */
            if (comp < 0 && n == p->left) {
                /* debug("Case 2.2 left-left"); */
                _rbt_node_rotate_right(T, p, NULL);
                _rbt_node_new(&n->left);
                n->left->parent = n;
                _rbt_node_set(T, n->left, v);
                n->color = BLACK;
                p->color = RED;
                n->left->color = RED;
            } else if (comp > 0 && n == p->right) {
                /* debug("Case 2.2 right-right"); */
                _rbt_node_rotate_left(T, p, NULL);
                _rbt_node_new(&n->right);
                n->right->parent = n;
                _rbt_node_set(T, n->right, v);
                n->color = BLACK;
                p->color = RED;
                n->right->color = RED;
            } else if (comp < 0 && n == p->right) {
                /* debug("Case 2.2 right-left"); */
                _rbt_node_new(&p->left);
                _rbt_node_set(T, p->left, p->data);
                _rbt_node_set(T, p, v);
                p->left->parent = p;
                p->left->color = RED;
            } else if (comp > 0 && n == p->left) {
                /* debug("Case 2.2 left-right"); */
                _rbt_node_new(&p->right);
                _rbt_node_set(T, p->right, p->data);
                _rbt_node_set(T, p, v);
                p->right->parent = p;
                p->right->color = RED;
            }
            ++T->size;
        }
    }
    return 0;
}

int _rbt_insert(_rbt *T, const void *v)
{
    /* debug("v = %d", *(int*)v); */
    check_ptr(T);
    check_ptr(v);
    assert (!_rbt_invariant(T));

    int rc = 0;

    if (!T->root) {
        rc = _rbt_node_new(&T->root);
        check(rc == 0, "_rbt_node_new failed.");
        rc = _rbt_node_set(T, T->root, v);
        check(rc == 0, "_rbt_node_set failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        rc = _rbt_node_insert(T, T->root, v);
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
                    assert((s->left && s->left->color == RED)
                            || (s->right && s->right->color == RED));

                    if (s->right && s->right->color == RED) {
                        /* Make sure the sibling has an outer (i.e. left) red child. */
                        rc = _rbt_node_rotate_left(T, s, &s);
                        check(rc == 0, "_rbt_node_rotate_left failed.");

                        /* Note that s has been updated to hold the new sibling. */
                        s->color = BLACK;
                        s->left->color = RED;
                    }

                    assert(s->left && s->left->color == RED);
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
                    if (s->left && s->left->color == RED) {
                        /* debug("... the left one, need to rotate it"); */
                        /* Make sure the sibling has an outer (i.e. right) red child. */
                        rc = _rbt_node_rotate_right(T, s, &s);
                        check(rc == 0, "_rbt_node_rotate_left failed.");

                        /* Note that s has been updated to hold the new sibling. */
                        s->color = BLACK;
                        s->right->color = RED;
                    }

                    assert(s->right && s->right->color == RED);
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

    assert((n->parent && (n == n->parent->left || n == n->parent->right))
           || n == T->root);

    int rc;

    int comp = TypeInterface_compare(T->element_type, value, n->data);

    /* Make sure we have leeway for the deletion: Need at least one red node in every
     * group we walk through. */
    if (_rbt_node_is_empty_group(n)) {
        ++_total;
        rc = _rbt_node_fill_group(T, n);
        check(rc == 0, "_rbt_node_fill_group failed.");
        if ((comp > 0 && _rbt_node_is_empty_group(n->right))
                || (comp < 0 && _rbt_node_is_empty_group(n->left))
                || comp == 0) {
            ++_needed;
        }
    }

    assert((n->color == BLACK
                && (((n->left && n->left->color == RED)
                        || (n->right && n->right->color == RED))
                    || n == T->root))
           || (n->color == RED && n->parent->color == BLACK));

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
                    --T->size;
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
            --T->size;
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
        rc = _rbt_node_remove(T, T->root, value);
        check(rc >= 0, "_rbt_node_insert failed.");
    }

    assert(!_rbt_invariant(T));
    return rc;
error:
    return -1;

}
