#include <assert.h>

#include "check.h"
#include "red_black_tree.h"

/* static inline */
RBTreeNode *RBTreeNode_new(void) { return calloc(1, sizeof(RBTreeNode)); }

/***************************************************************************************
 *
 * int RBTreeNode_set_key(const RBTree *T, RBTreeNode *n, const void *k)
 *
 * Set the key of n to the value of the key object pointed to by k.
 * Since there is no use case where it makes sense to overwrite an existing key, we
 * assume that no key is stored in n.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 * TODO: What about swapping nodes when deleting entries? => Swap RBTreeData objects
 * wholesale.
 *
 **************************************************************************************/

int RBTreeNode_set_key(const RBTree *T, RBTreeNode *n, const void *k)
{
    int rc = MappingData_set_key(&n->data, T->external_storage, T->key_type, k);
    check(rc == 0, "Failed to set key.");
    n->has_key = 1;
    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * void RBTreeNode_destroy_key(RBTree *T, RBTreeNode *n);
 *
 * Destroy the key stored in n, freeing memory on the heap if necessary.
 *
 **************************************************************************************/

void RBTreeNode_destroy_key(RBTree *T, RBTreeNode *n)
{
    MappingData_destroy_key(&n->data, T->external_storage, T->key_type);
    n->has_key = 0;
}

/***************************************************************************************
 *
 * int RBTreeNode_set_value(const RBTree *T, RBTreeNode *n, const void *v)
 *
 * Set the the value part of n to the value of the object pointed to by v.
 * Other than with keys, it makes sense to overwrite existing values.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int RBTreeNode_set_value(const RBTree *T, RBTreeNode *n, const void *v)
{
    int rc = MappingData_set_value(&n->data, T->external_storage,
                                   T->key_type, T->value_type, v);
    check(rc == 0, "Failed to set value.");
    n->has_key = 1;
    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * void RBTreeNode_destroy_value(RBTree *T, RBTreeNode *n);
 *
 * Destroy the value stored in n, freeing memory on the heap if necessary.
 *
 **************************************************************************************/

void RBTreeNode_destroy_value(RBTree *T, RBTreeNode *n)
{
    MappingData_destroy_value(&n->data, T->external_storage,
                              T->key_type, T->value_type);
    n->has_value = 0;
}

/***************************************************************************************
 *
 * void RBTreeNode_delete(const RBTree *T, RBTreeNode *n);
 *
 * Delete n, freeing any associated memory.
 * No edges are removed by this function, this is the responsibility of the caller.
 *
 **************************************************************************************/

/* static inline */
void RBTreeNode_delete(RBTree *T, RBTreeNode *n)
{
    if (n) {
        RBTreeNode_destroy_key(T, n);
        if (T->value_type) RBTreeNode_destroy_value(T, n);
        free(n);
    }
}

/**************************************************************************************
 *
 * RBTreeNode *RBTreeNode_copy(const RBTree *T, RBTreeNode *n)
 *
 * Recursively copy the tree rooted at src. Store a pointer to the copy at the pointer
 * location pointed to by dest.
 *
 *************************************************************************************/

int RBTreeNode_copy(RBTree *T, RBTreeNode **dest, RBTreeNode *src)
{
    check_ptr(T);
    if (src == NULL) {
        *dest = NULL;
        return 0;
    }

    int rc;

    RBTreeNode *c = RBTreeNode_new();
    check(c, "Failed to create new node.");

    rc = RBTreeNode_set_key(T, c, RBTreeNode_key_address(T, src));
    check(rc == 0, "Failed to copy key to new node.");

    if (T->value_type) {
        rc = RBTreeNode_set_value(T, c, RBTreeNode_value_address(T, src));
        check(rc == 0, "Failed to copy key to new node.");
    }

    c->color = src->color;

    rc = RBTreeNode_copy(T, &c->left, src->left);
    check(rc == 0, "Failed to copy left subtree.");
    if (c->left) c->left->parent = c;

    rc = RBTreeNode_copy(T, &c->right, src->right);
    check(rc == 0, "Failed to copy right subtree.");
    if (c->right) c->right->parent = c;

    *dest = c;
    return 0;
error:
    if (c) RBTreeNode_delete(T, c);
    return -1;
}

/* Replace the child in a node after the child was rotated. */
static inline
void RBTreeNode_replace_child(RBTree *T,
                             RBTreeNode *parent,
                             RBTreeNode *old_child,
                             RBTreeNode *new_child)
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
int RBTreeNode_rotate_left(RBTree *T, RBTreeNode *n, RBTreeNode **node_out)
{
    check_ptr(n);
    assert(n->right);

    RBTreeNode *p  = n->parent;
    RBTreeNode *r  = n->right;
    RBTreeNode *rl = r->left;

    r->left = n, n->parent = r;
    n->right = rl;
    if (rl) rl->parent = n;
    RBTreeNode_replace_child(T, p, n, r);

    if (node_out) *node_out = r;
    return 0;
error:
    return -1;
}

/* static */
int RBTreeNode_rotate_right(RBTree *T, RBTreeNode *n, RBTreeNode **node_out)
{
    check_ptr(n);
    assert(n->left);

    RBTreeNode *p  = n->parent;
    RBTreeNode *l  = n->left;
    RBTreeNode *lr = l->right;

    l->right = n, n->parent = l;
    n->left = lr;
    if (lr) lr->parent = n;
    RBTreeNode_replace_child(T, p, n, l);

    if (node_out) *node_out = l;
    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * int RBTree_initialize(RBTree *T, t_intf *key_type, t_intf *value_type);
 *
 * Initialize T, which is assumed to be a pointer to a memory location with enough space
 * for an RBTree. The key type is mandatory, the value type can be NULL if single
 * objects instead of key-value pairs should be stored.
 *
 * If the combined size of the key and value elements is less than RBT_ALLOC_THRESHOLD,
 * both will be stored within the nodes themselves. Otherwise they're allocated and
 * referenced with pointers.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int RBTree_initialize(RBTree *T, t_intf *key_type, t_intf *value_type)
{
    check_ptr(T);
    check_ptr(key_type);
    check(key_type->compare, "No comparison function was given.");

    T->root = NULL;
    T->size = 0;
    T->key_type = key_type;
    T->value_type = value_type;

    if ( t_size(key_type)
            + ( value_type ? t_size(value_type) : 0 )
            > RBT_ALLOC_THRESHOLD ) {
        T->external_storage = 1;
    } else {
        T->external_storage = 0;
    }

    return 0;
error:
    return -1;
}

static inline void RBTreeNode_clear(RBTree *T, RBTreeNode *n)
{
    if (n) {
        if (n->left) RBTreeNode_clear(T, n->left);
        if (n->right) RBTreeNode_clear(T, n->right);
        RBTreeNode_delete(T, n);
    }
}

void RBTree_clear(RBTree *T)
{
    if (T) RBTreeNode_clear(T, T->root);
    T->root = NULL;
    T->size = 0;
}

/**************************************************************************************
 *
 * int RBTree_copy(RBTree *dest, const RBTree *src)
 *
 * Recursively copy a tree from src to dest.
 * Assumes that dest points to an initialized rbt.
 *
 *************************************************************************************/

int RBTree_copy(RBTree *dest, const RBTree *src)
{
    check_ptr(dest);
    check_ptr(src);
    assert(!RBTree_invariant(src));

    int rc;

    if (dest->size > 0) RBTree_clear(dest);
    dest->key_type = src->key_type;

    if (src->size > 0) {
        rc = RBTreeNode_copy(dest, &dest->root, src->root);
        check(rc == 0, "Failed to copy nodes.");
        dest->root->parent = NULL;
        dest->size = src->size;
    }

    assert(!RBTree_invariant(dest));
    return 0;
error:
    return -1;
}

/* Return the weight of the group that contains n. n can be any node in the group. */
static inline unsigned short RBTree_group_weight(RBTreeNode *n)
{
    if (!n) return 0;
    if (n->color == RED) n = n->parent;
    unsigned short w = 1;
    if (n->left && n->left->color == RED) ++w;
    if (n->right && n->right->color == RED) ++w;
    return w;
}

static void RBTree_group_decrease_weight(RBTree *T, RBTreeNode *n)
{
    RBTreeNode *p = n->parent;
    RBTreeNode *pp = p ? p->parent : NULL;
    RBTreeNode *l = n->left;
    RBTreeNode *r = n->right;

    assert(RBTree_group_weight(n) == 3);

    /* Case 1: n is the root of the tree. */
    if (n == T->root) {
        /* log_info("Case 1: n is the root"); */
        l->color = r->color = BLACK;
    }

    /* Case 2: p is black. */
    else if (p->color == BLACK) {
        /* log_info("Case 2: p is black"); */
        n->color = RED;
        l->color = r->color = BLACK;
    }

    /* Case 3: p is red. */
    else if (p->color == RED) {

        /* Case 3.1: pp has 1 red child (p) */
        RBTreeNode *ps = p == pp->left ? pp->right : pp->left;
        assert(ps);
        if (ps->color == BLACK) {

            /* Case 3.1a: left-left or right-right ancestor chain. */
            if (n == pp->right->right || n == pp->left->left) {
                /* log_info("Case 3.1a straight ancestor chain"); */
                if (n == pp->right->right) {
                    RBTreeNode_rotate_left(T, pp, NULL);
                } else {
                    RBTreeNode_rotate_right(T, pp, NULL);
                }
                pp->color = RED;
                p->color = BLACK;
                n->color = RED;
                l->color = r->color = BLACK;
            }

            /* Case 3.1b: right-left or left-right ancestor chain. */
            else {
                /* log_info("Case 3.1b: right-left or left-right ancestor chain"); */
                if (n == pp->right->left) {
                    RBTreeNode_rotate_right(T, p, NULL);
                    RBTreeNode_rotate_left(T, pp, NULL);
                } else {
                    RBTreeNode_rotate_left(T, p, NULL);
                    RBTreeNode_rotate_right(T, pp, NULL);
                }
                pp->color = RED;
                l->color = r->color = BLACK;
            }
        }

        /* Case 3.2: pp has 2 red children */
        else {
            /* log_info("Case 3.2: pp is full."); */
            RBTree_group_decrease_weight(T, pp);
            /* Things may have changed. Go again. */
            RBTree_group_decrease_weight(T, n);
        }
    }
}

static int RBTreeNode_insert(RBTree *T, RBTreeNode *n, const void *k)
{
    int comp = t_compare(T->key_type, k, RBTreeNode_key_address(T, n));
    if (comp == 0) {
        return 1; /* found it, nothing to do */
    } else if (comp < 0 && n->left) {
        return RBTreeNode_insert(T, n->left, k);
    } else if (comp > 0 && n->right) {
        return RBTreeNode_insert(T, n->right, k);
    }

    /* Case 1: n is black */
    if (n->color == BLACK) {
        if (comp < 0) {
            /* log_info("Case 1 left"); */
            n->left = RBTreeNode_new();
            RBTreeNode_set_key(T, n->left, k);
            n->left->parent = n;
            n->left->color = RED;
            ++T->size;
        } else { /* comp > 0 */
            /* log_info("Case 1 right"); */
            n->right = RBTreeNode_new();
            RBTreeNode_set_key(T, n->right, k);
            n->right->parent = n;
            n->right->color = RED;
            ++T->size;
        }
    }

    /* Case 2: n is red */
    else {
        RBTreeNode *p = n->parent;

        /* Case 2.1: p has 2 red children */
        if (RBTree_group_weight(p) == 3) {
            /* log_info("Case 2.1 p is full"); */
            RBTree_group_decrease_weight(T, p);
            /* Now n may be elsewhere. Go again. */
            return RBTreeNode_insert(T, n, k);
        }

        /* Case 2.2: p has 1 red child (n) */
        else { /* weight(p) = 1 is handled in case 1 */
            if (comp < 0 && n == p->left) {
                /* log_info("Case 2.2 left-left"); */
                RBTreeNode_rotate_right(T, p, NULL);
                n->left = RBTreeNode_new();
                n->left->parent = n;
                RBTreeNode_set_key(T, n->left, k);
                n->color = BLACK;
                p->color = RED;
                n->left->color = RED;
            } else if (comp > 0 && n == p->right) {
                /* log_info("Case 2.2 right-right"); */
                RBTreeNode_rotate_left(T, p, NULL);
                n->right = RBTreeNode_new();
                n->right->parent = n;
                RBTreeNode_set_key(T, n->right, k);
                n->color = BLACK;
                p->color = RED;
                n->right->color = RED;
            } else if (comp < 0 && n == p->right) {
                /* log_info("Case 2.2 right-left"); */
                p->left = RBTreeNode_new();
                RBTreeNode_set_key(T, p->left, RBTreeNode_key_address(T, p));
                RBTreeNode_set_key(T, p, k);
                p->left->parent = p;
                p->left->color = RED;
            } else if (comp > 0 && n == p->left) {
                /* log_info("Case 2.2 left-right"); */
                p->right = RBTreeNode_new();
                RBTreeNode_set_key(T, p->right, RBTreeNode_key_address(T, p));
                RBTreeNode_set_key(T, p, k);
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
 * int RBTree_insert(RBTree *T, const void *k)
 *
 * Insert an element with the key k into the tree T.
 *
 * Return values:
 *      1: The element was found and not added.
 *      0: The element was added.
 *     -1: An error occured.
 *
 *************************************************************************************/

int RBTree_insert(RBTree *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    assert (!RBTree_invariant(T));

    int rc = 0;

    if (!T->root) {
        T->root = RBTreeNode_new();
        check(T->root != NULL, "RBTreeNode_new failed.");
        rc = RBTreeNode_set_key(T, T->root, k);
        check(rc == 0, "RBTreeNode_set_key failed.");
        T->root->color = BLACK;
        ++T->size;
    } else {
        rc = RBTreeNode_insert(T, T->root, k);
        check(rc >= 0, "RBTreeNode_insert failed.");
    }

    assert(!RBTree_invariant(T));
    return rc;
error:
    return -1;
}

int RBTree_has(const RBTree *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);

    RBTreeNode *n = T->root;
    int comp;

    while (n) {
        comp = t_compare(T->key_type, k, RBTreeNode_key_address(T, n));
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
 * static void RBTree_group_increase_weight(RBTree *T, RBTreeNode *n)
 *
 * Increase the weight of the group with the head n. It is assumed that n is the head of
 * an empty group, i.o.w. n->color == BLACK && !n->left && !n->right.  After the
 * function exits n can be both red or black.
 *
 * See ./doc/red_black_tree.pdf for a description of the algorithm.
 *
 **************************************************************************************/

static void RBTree_group_increase_weight(RBTree *T, RBTreeNode *n)
{
    RBTreeNode *p = n->parent;
    RBTreeNode *s = p ? (n == p->left ? p->right : p->left) : NULL;
    RBTreeNode *so = NULL; /* outer child of s */

    /* Case 1: ancestor group is not empty. */
    if (RBTree_group_weight(p) > 1) {
        if (p->color == BLACK) {
            if (n == p->right) {
                RBTreeNode_rotate_right(T, p, NULL);
                s = p->left;
            } else {
                RBTreeNode_rotate_left(T, p, NULL);
                s = p->right;
            }
            p->parent->color = BLACK;
            p->color = RED;
        }

        assert(p->color == RED);

        /* Case 1.1: s is empty. */
        if (RBTree_group_weight(s) == 1) {
            p->color = BLACK;
            n->color = s->color = RED;
        }

        /* Case 1.2: s is not empty. */
        else {
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || s->left->color == BLACK)) {
                RBTreeNode_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                s->color = BLACK;
                so->color = RED;
            } else if (s == p->right && (!s->right || s->right->color == BLACK)) {
                RBTreeNode_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                s->color = BLACK;
                so->color = RED;
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                RBTreeNode_rotate_right(T, p, NULL);
            } else {
                RBTreeNode_rotate_left(T, p, NULL);
            }

            p->color = BLACK;
            n->color = s->color = RED;
            so->color = BLACK;
        }
    }

    /* Case 2: ancestor group is empty. */
    else { /* RBTree_group_weight(p) <= 1 */

        /* Case 2.1: s is empty. */
        if (RBTree_group_weight(s) == 1) {
            if (p == T->root) {
                n->color = s->color = RED;
            } else {
                RBTree_group_increase_weight(T, p);
                return RBTree_group_increase_weight(T, n);
            }
        }

        /* Case 2.2: s is not empty. Like 1.2 with different color changes. */
        else { /* RBTree_group_weight(s) > 1 */
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || s->left->color == BLACK)) {
                RBTreeNode_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                s->color = BLACK;
                so->color = RED;
            } else if (s == p->right && (!s->right || s->right->color == BLACK)) {
                RBTreeNode_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                s->color = BLACK;
                so->color = RED;
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                RBTreeNode_rotate_right(T, p, NULL);
            } else {
                RBTreeNode_rotate_left(T, p, NULL);
            }

            n->color = RED;
            so->color = BLACK;
        }
    }
}

int RBTreeNode_remove(RBTree *T, RBTreeNode *n, const void *k)
{
    int comp;
    for ( ;; ) {
        if (!n) return 0; /* not found */
        comp = t_compare(T->key_type, k, RBTreeNode_key_address(T, n));
        if (comp == 0) break; /* found, go on */
        else if (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
    }

    if (n->left && n->right) {
        /* n has two children. Find the node with the smallest key greater than the key
         * of n. */
        RBTreeNode *succ = n->right;
        while (succ->left) succ = succ->left;
        /* The normal approach is to swap the payloads of n and succ at this point.
         * However, we don't need succ->data to remove succ later, so we destroy n's
         * payload now, move over succ->data to n wholesale, zero out succ->data and
         * move on to remove succ (then stored in n) from the tree. */
        RBTreeNode_destroy_key(T, n);
        /* TODO: bundle both in RBTreeNode_destroy_data? */
        if (T->value_type) RBTreeNode_destroy_value(T, n);
        memmove(&n->data, &succ->data, sizeof(union MappingData));
        memset(&succ->data, 0, sizeof(union MappingData));
        n = succ;
    }

    assert(!n->right || !n->left);

    if (n->color == BLACK && n != T->root && RBTree_group_weight(n) == 1) {
        RBTree_group_increase_weight(T, n);
        assert(RBTree_group_weight(n) > 1);
    }

    if (n->color == BLACK) {
        if (n->left) {
            RBTreeNode_rotate_right(T, n, NULL);
            n->color = RED;
            n->parent->color = BLACK;
        } else if (n->right) {
            RBTreeNode_rotate_left(T, n, NULL);
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
    RBTreeNode_delete(T, n);
    --T->size;

    return 1; /* found it, deleted it */
}

/**************************************************************************************
 *
 * int RBTree_remove(RBTree *T, const void *k)
 *
 * Remove the element with the value k from the tree T.
 *
 * Return values:
 *      1: The element was found and deleted.
 *      0: The element was not found.
 *     -1: An error occured.
 *
 *************************************************************************************/

int RBTree_remove(RBTree *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    assert (!RBTree_invariant(T));

    int rc = 0;

    if (T->root) rc = RBTreeNode_remove(T, T->root, k);

    assert(!RBTree_invariant(T));
    return rc;
error:
    return -1;

}

/***************************************************************************************
 *
 * static int RBTreeNode_traverse(RBTreeNode *n,
 *                               int (*f)(RBTreeNode *n, void *p),
 *                               void *p);
 * static int RBTreeNode_traverse_r(RBTreeNode *n,
 *                                 int (*f)(RBTreeNode *n, void *p),
 *                                 void *p);
 * static int RBTreeNode_traverse_keys(RBTree *T, RBTreeNode *n,
 *                                     int (*f)(void *k, void *p),
 *                                     void *p);
 * static int RBTreeNode_traverse_keys_r(RBTree *T, RBTreeNode *n,
 *                                       int (*f)(void *k, void *p),
 *                                       void *p);
 * static int RBTreeNode_traverse_values(RBTree *T, RBTreeNode *n,
 *                                       int (*f)(void *v, void *p),
 *                                       void *p);
 * static int RBTreeNode_traverse_values_r(RBTree *T, RBTreeNode *n,
 *                                         int (*f)(void *v, void *p),
 *                                         void *p);
 *
 * Walk through all the nodes of the sub-tree with the root n in ascending/descending
 * order. Call f on every node, key, or value with the additional parameter p. If f
 * returns a non-zero integer, abort and return it.
 *
 * These functions are called by their counterparts RBTree_traverse... to do the actual
 * work. There should be no need to call them directly from the outside.
 *
 **************************************************************************************/

static int RBTreeNode_traverse(RBTreeNode *n,
                              int (*f)(RBTreeNode *n, void *p),
                              void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = RBTreeNode_traverse(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = RBTreeNode_traverse(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

static int RBTreeNode_traverse_r(RBTreeNode *n,
                                int (*f)(RBTreeNode *n, void *p),
                                void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = RBTreeNode_traverse_r(n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = RBTreeNode_traverse_r(n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

static int RBTreeNode_traverse_keys(RBTree *T, RBTreeNode *n,
                                    int (*f)(void *k, void *p),
                                    void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = RBTreeNode_traverse_keys(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(RBTreeNode_key_address(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = RBTreeNode_traverse_keys(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

static int RBTreeNode_traverse_keys_r(RBTree *T, RBTreeNode *n,
                                      int (*f)(void *k, void *p),
                                      void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = RBTreeNode_traverse_keys_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(RBTreeNode_key_address(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = RBTreeNode_traverse_keys_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

static int RBTreeNode_traverse_values(RBTree *T, RBTreeNode *n,
                                      int (*f)(void *v, void *p),
                                      void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = RBTreeNode_traverse_values(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(RBTreeNode_value_address(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = RBTreeNode_traverse_values(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

static int RBTreeNode_traverse_values_r(RBTree *T, RBTreeNode *n,
                                        int (*f)(void *v, void *p),
                                        void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = RBTreeNode_traverse_values_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(RBTreeNode_value_address(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = RBTreeNode_traverse_values_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

/***************************************************************************************
 *
 * int RBTree_traverse_nodes(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);
 * int RBTree_traverse_nodes_r(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);
 * int RBTree_traverse_keys(RBTree *T, int (*f)(void *k, void *p), void *p);
 * int RBTree_traverse_keys_r(RBTree *T, int (*f)(void *k, void *p), void *p);
 * int RBTree_traverse_values(RBTree *T, int (*f)(void *v, void *p), void *p);
 * int RBTree_traverse_values_r(RBTree *T, int (*f)(void *v, void *p), void *p);
 *
 * Walk through all the nodes of the tree in ascending/descending order.  Call f on
 * every node, key, or value with the additional parameter p. If f returns a non-zero
 * integer, abort and return it.
 *
 **************************************************************************************/

int RBTree_traverse_nodes(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p) {
    if (T && T->root) {
        return RBTreeNode_traverse(T->root, f, p);
    }
    return 0;
}

int RBTree_traverse_nodes_r(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p) {
    if (T && T->root) {
        return RBTreeNode_traverse_r(T->root, f, p);
    }
    return 0;
}

int RBTree_traverse_keys(RBTree *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) {
        return RBTreeNode_traverse_keys(T, T->root, f, p);
    }
    return 0;
}

int RBTree_traverse_keys_r(RBTree *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) {
        return RBTreeNode_traverse_keys_r(T, T->root, f, p);
    }
    return 0;
}

int RBTree_traverse_values(RBTree *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "This tree doesn't store values."
                         "Did you mean to traverse the keys?")
    if (T && T->root) {
        return RBTreeNode_traverse_values(T, T->root, f, p);
    }
    return 0;
error:
    return -1;
}

int RBTree_traverse_values_r(RBTree *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "This tree doesn't store values."
                         "Did you mean to traverse the keys?")
    if (T && T->root) {
        return RBTreeNode_traverse_values_r(T, T->root, f, p);
    }
    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * int RBTreeNode_invariant(RBTreeNode *n, void *black_count);
 *
 * Check if n violates any of the red-black tree invariants. Traversal callback for
 * RBTree_invariant.
 *
 * Return values: 0 on success
 *               -1 if two adjacent red nodes are detected
 *               -2 if n violates the same-depth of leaf nodes property
 *
 **************************************************************************************/

int RBTreeNode_invariant(RBTreeNode *n, void *black_count)
{
    if (n->color == RED) {
        /* Check for adjacent red nodes. */
        if (n->parent && n->parent->color == RED) {
            log_info("Invariant violated: Two adjacent red nodes.")
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
            log_info("Invariant violated: Unequal numbers of black nodes per path.");
            return -2;
        }
    }

    return 0;
}

/***************************************************************************************
 *
 * int RBTree_invariant(RBTree *T);
 *
 * Check if T is still intact and satisfies the red-black tree properties.
 *
 * Return values: 0 on success
 *               -1 if two adjacent red nodes are detected
 *               -2 if the same-depth of leaf nodes property is violated
 *
 **************************************************************************************/

int RBTree_invariant(const RBTree *T)
{
    int black_count = -1;
    int rc = RBTreeNode_traverse(T->root, RBTreeNode_invariant, &black_count);
    return rc;
}
