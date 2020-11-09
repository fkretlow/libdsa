#include <assert.h>

#include "debug.h"
#include "rbt.h"

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
_rbt_node *_rbt_node_new(void)
{
    return calloc(1, sizeof(_rbt_node));
}

/* static inline */
void _rbt_node_delete(const _rbt *T, _rbt_node *n)
{
    /* We don't remove any edges here. */
    if (n) {
        if (n->data && T) {
            TypeInterface_destroy(T->element_type, n->data);
        }
        free(n->data);
        free(n);
    }
}

/* static */
int _rbt_node_set(const _rbt *T, _rbt_node *n, const void *v)
{
    check_ptr(T);
    check_ptr(n);
    check_ptr(v);

    if (n->data) {
        TypeInterface_destroy(T->element_type, n->data);
    } else {
        n->data = TypeInterface_allocate(T->element_type, 1);
        check_alloc(n->data);
    }

    TypeInterface_copy(T->element_type, n->data, v);

    return 0;
error:
    return -1;
}

/**************************************************************************************
 *
 * _rbt_node *_rbt_node_copy(const _rbt *T, const _rbt_node *n)
 *
 * Recursively copy the tree rooted at src to dest.
 *
 *************************************************************************************/

int _rbt_node_copy(const _rbt *T, _rbt_node **dest, const _rbt_node *src)
{
    check_ptr(T);
    if (src == NULL) {
        *dest = NULL;
        return 0;
    }

    int rc;

    _rbt_node *c = _rbt_node_new();
    check(c, "Failed to create new node.");

    rc = _rbt_node_set(T, c, src->data);
    check(rc == 0, "Failed to copy data to new node.");
    c->color = src->color;

    rc = _rbt_node_copy(T, &c->left, src->left);
    check(rc == 0, "Failed to copy left subtree.");
    if (c->left) c->left->parent = c;

    rc = _rbt_node_copy(T, &c->right, src->right);
    check(rc == 0, "Failed to copy right subtree.");
    if (c->right) c->right->parent = c;

    *dest = c;
    return 0;
error:
    if (c) _rbt_node_delete(T, c);
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

/**************************************************************************************
 *
 * int _rbt_copy(_rbt *dest, const _rbt *src)
 *
 * Recursively copy a tree from src to dest.
 * Assumes that dest points to an initialized rbt.
 *
 *************************************************************************************/

int _rbt_copy(_rbt *dest, const _rbt *src)
{
    check_ptr(dest);
    check_ptr(src);
    assert(!_rbt_invariant(src));

    int rc;

    if (dest->size > 0) _rbt_clear(dest);
    dest->element_type = src->element_type;

    if (src->size > 0) {
        rc = _rbt_node_copy(dest, &dest->root, src->root);
        check(rc == 0, "Failed to copy nodes.");
        dest->root->parent = NULL;
        dest->size = src->size;
    }

    assert(!_rbt_invariant(dest));
    return 0;
error:
    return -1;
}

/* Return the weight of the group that contains n. n can be any node in the group. */
static inline unsigned short _rbt_group_weight(_rbt_node *n)
{
    if (!n) return 0;
    if (n->color == RED) n = n->parent;
    unsigned short w = 1;
    if (n->left && n->left->color == RED) ++w;
    if (n->right && n->right->color == RED) ++w;
    return w;
}

static void _rbt_group_decrease_weight(_rbt *T, _rbt_node *n)
{
    _rbt_node *p = n->parent;
    _rbt_node *pp = p ? p->parent : NULL;
    _rbt_node *l = n->left;
    _rbt_node *r = n->right;

    assert(_rbt_group_weight(n) == 3);

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
        return 1; /* found it, nothing to do */
    } else if (comp < 0 && n->left) {
        return _rbt_node_insert(T, n->left, v);
    } else if (comp > 0 && n->right) {
        return _rbt_node_insert(T, n->right, v);
    }

    /* Case 1: n is black */
    if (n->color == BLACK) {
        if (comp < 0) {
            /* debug("Case 1 left"); */
            n->left = _rbt_node_new();
            _rbt_node_set(T, n->left, v);
            n->left->parent = n;
            n->left->color = RED;
            ++T->size;
        } else { /* comp > 0 */
            /* debug("Case 1 right"); */
            n->right = _rbt_node_new();
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
        if (_rbt_group_weight(p) == 3) {
            /* debug("Case 2.1 p is full"); */
            _rbt_group_decrease_weight(T, p);
            /* Now n may be elsewhere. Go again. */
            return _rbt_node_insert(T, n, v);
        }

        /* Case 2.2: p has 1 red child (n) */
        else { /* weight(p) = 1 is handled in case 1 */
            if (comp < 0 && n == p->left) {
                /* debug("Case 2.2 left-left"); */
                _rbt_node_rotate_right(T, p, NULL);
                n->left = _rbt_node_new();
                n->left->parent = n;
                _rbt_node_set(T, n->left, v);
                n->color = BLACK;
                p->color = RED;
                n->left->color = RED;
            } else if (comp > 0 && n == p->right) {
                /* debug("Case 2.2 right-right"); */
                _rbt_node_rotate_left(T, p, NULL);
                n->right = _rbt_node_new();
                n->right->parent = n;
                _rbt_node_set(T, n->right, v);
                n->color = BLACK;
                p->color = RED;
                n->right->color = RED;
            } else if (comp < 0 && n == p->right) {
                /* debug("Case 2.2 right-left"); */
                p->left = _rbt_node_new();
                _rbt_node_set(T, p->left, p->data);
                _rbt_node_set(T, p, v);
                p->left->parent = p;
                p->left->color = RED;
            } else if (comp > 0 && n == p->left) {
                /* debug("Case 2.2 left-right"); */
                p->right = _rbt_node_new();
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

/**************************************************************************************
 *
 * int _rbt_insert(_rbt *T, const void *v)
 *
 * Insert an element with the value v into the tree T.
 *
 * Return values:
 *      1: The element was found and not added.
 *      0: The element was added.
 *     -1: An error occured.
 *
 *************************************************************************************/

int _rbt_insert(_rbt *T, const void *v)
{
    /* debug("v = %d", *(int*)v); */
    check_ptr(T);
    check_ptr(v);
    assert (!_rbt_invariant(T));

    int rc = 0;

    if (!T->root) {
        T->root = _rbt_node_new();
        check(T->root != NULL, "_rbt_node_new failed.");
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

int _rbt_has(const _rbt *T, const void *v)
{
    check_ptr(T);
    check_ptr(v);

    _rbt_node *n = T->root;
    int comp;

    while (n) {
        comp = TypeInterface_compare(T->element_type, v, n->data);
        if (comp > 0) {
            n = n->right;
        } else if (comp < 0) {
            n = n->left;
        } else { /* comp == 0: found it. */
            return 1;
        }
    }

    /* Didn't find it. */
    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * static void _rbt_group_increase_weight(_rbt *T, _rbt_node *n)
 *
 * Increase the weight of the group with the head n. It is assumed that n is the head of
 * an empty group, i.o.w. n->color == BLACK && !n->left && !n->right.  After the
 * function exits n can be both red or black.
 *
 * See ./doc/red_black_tree.pdf for a description of the algorithm.
 *
 **************************************************************************************/

static void _rbt_group_increase_weight(_rbt *T, _rbt_node *n)
{
    _rbt_node *p = n->parent;
    _rbt_node *s = p ? (n == p->left ? p->right : p->left) : NULL;
    _rbt_node *so = NULL; /* outer child of s */

    /* Case 1: ancestor group is not empty. */
    if (_rbt_group_weight(p) > 1) {
        if (p->color == BLACK) {
            if (n == p->right) {
                _rbt_node_rotate_right(T, p, NULL);
                s = p->left;
            } else {
                _rbt_node_rotate_left(T, p, NULL);
                s = p->right;
            }
            p->parent->color = BLACK;
            p->color = RED;
        }

        assert(p->color == RED);

        /* Case 1.1: s is empty. */
        if (_rbt_group_weight(s) == 1) {
            p->color = BLACK;
            n->color = s->color = RED;
        }

        /* Case 1.2: s is not empty. */
        else {
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || s->left->color == BLACK)) {
                _rbt_node_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                s->color = BLACK;
                so->color = RED;
            } else if (s == p->right && (!s->right || s->right->color == BLACK)) {
                _rbt_node_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                s->color = BLACK;
                so->color = RED;
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                _rbt_node_rotate_right(T, p, NULL);
            } else {
                _rbt_node_rotate_left(T, p, NULL);
            }

            p->color = BLACK;
            n->color = s->color = RED;
            so->color = BLACK;
        }
    }

    /* Case 2: ancestor group is empty. */
    else { /* _rbt_group_weight(p) <= 1 */

        /* Case 2.1: s is empty. */
        if (_rbt_group_weight(s) == 1) {
            if (p == T->root) {
                n->color = s->color = RED;
            } else {
                _rbt_group_increase_weight(T, p);
                return _rbt_group_increase_weight(T, n);
            }
        }

        /* Case 2.2: s is not empty. Like 1.2 with different color changes. */
        else { /* _rbt_group_weight(s) > 1 */
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || s->left->color == BLACK)) {
                _rbt_node_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                s->color = BLACK;
                so->color = RED;
            } else if (s == p->right && (!s->right || s->right->color == BLACK)) {
                _rbt_node_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                s->color = BLACK;
                so->color = RED;
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                _rbt_node_rotate_right(T, p, NULL);
            } else {
                _rbt_node_rotate_left(T, p, NULL);
            }

            n->color = RED;
            so->color = BLACK;
        }
    }
}

int _rbt_node_remove(_rbt *T, _rbt_node *n, const void *v)
{
    int comp;
    for ( ;; ) {
        if (!n) return 0; /* not found */
        comp = TypeInterface_compare(T->element_type, v, n->data);
        if (comp == 0) break; /* found, go on */
        else if (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
    }

    if (n->left && n->right) {
        /* n is not in a leaf position. Swap with a node that is. */
        _rbt_node *succ = n->right;
        while (succ->left) succ = succ->left;
        /* succ->data isn't needed for the deletion. Set it to NULL so the value isn't
         * destroyed. This way we save a call to the value copy constructor. */
        TypeInterface_destroy(T->element_type, n->data);
        free(n->data);
        n->data = succ->data;
        succ->data = NULL;
        n = succ;
    }

    assert(!n->right || !n->left);

    if (n->color == BLACK && n != T->root && _rbt_group_weight(n) == 1) {
        _rbt_group_increase_weight(T, n);
        assert(_rbt_group_weight(n) > 1);
    }

    if (n->color == BLACK) {
        if (n->left) {
            _rbt_node_rotate_right(T, n, NULL);
            n->color = RED;
            n->parent->color = BLACK;
        } else if (n->right) {
            _rbt_node_rotate_left(T, n, NULL);
            n->color = RED;
            n->parent->color = BLACK;
        }
    }

    if (n == T->root) {
        T->root = NULL;
    } else if (n == n->parent->left) {
        n->parent->left = NULL;
    } else {
        n->parent->right = NULL;
    }
    _rbt_node_delete(T, n);
    --T->size;

    return 1; /* found it, deleted it */
}

/**************************************************************************************
 *
 * int _rbt_remove(_rbt *T, const void *v)
 *
 * Remove the element with the value v from the tree T.
 *
 * Return values:
 *      1: The element was found and deleted.
 *      0: The element was not found.
 *     -1: An error occured.
 *
 *************************************************************************************/

int _rbt_remove(_rbt *T, const void *v)
{
    /* debug("v = %d", *(int*)v); */
    check_ptr(T);
    check_ptr(v);
    assert (!_rbt_invariant(T));

    int rc = 0;

    if (T->root) rc = _rbt_node_remove(T, T->root, v);

    assert(!_rbt_invariant(T));
    return rc;
error:
    return -1;

}
