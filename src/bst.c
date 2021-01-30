/*************************************************************************************************
 *
 * bst.c
 *
 * This file provides the complete implementation for a classic binary search tree that supports
 * different key/value types by way of type interface structs, with additional hooks for different
 * insertion/deletion algorithms depending on whether one of the available balancing strategies is
 * selected for the tree (left-leaning red-black (2-3) tree, implementation in rb.c; or AVL tree,
 * in avl.c).
 *
 * The implementation is somewhat dauntless: no data fields are defined in the node struct, but
 * enough space is dynamically allocated for every node depending on the type interfaces stored
 * with the tree. On access, the correct offset is computed. This works because `free` doesn't
 * depend on size information stored with the type of its argument. However, I wouldn't be
 * surprised if this turned out to be horribly insecure. At any rate, you are hereby discouraged
 * from messing around with `bst_n`s in your own code... ;)
 *
 * We follow a model where recursive calls would look like this in more abstract languages:
 * `node.left = insert(node.left, key)`, where `insert` would return the new root of the subtree
 * after the insertion (which might be unchanged). Creating a new node in C requires a call to
 * `malloc` at the end of the call chain, which can fail. So we need a way to signal failure up
 * the chain without breaking previous links. This is done with integer return codes. As a nice
 * benefit, this provides a way to inform the caller if the number of nodes in the tree has
 * actually changed. In order to be able to update child pointers, we make use of pointer to
 * pointer semantics. The equivalent to the above snippet would be something like this: `int rc =
 * insert(&node->left, key)`, where `insert` saves the address of the new root of the subtree at
 * the address of node->left.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "bst.h"
#include "log.h"

/* bst_n *bst_n_new(const bst *T, const void *k, const void *v)
 * Create a new node with the key k and the value v (if given) on the heap and return a pointer to
 * it, or NULL on error. Enough memory is requested to store the node header, one key, and zero or
 * one value objects according to the type interfaces stored in T.
 * Note that new RB nodes are always red and RED = 0, so as long as we're using `calloc` to
 * allocate the node, there's no need to explicitly set the color. */
bst_n *bst_n_new(const bst *T, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    size_t size = bst_n_size(T);
    bst_n *n = calloc(1, size);
    check_alloc(n);

    t_copy(T->key_type, bst_n_key(T, n), k);
    n->flags.plain.has_key = 1;
    if (v) {
        t_copy(T->value_type, bst_n_value(T, n), v);
        n->flags.plain.has_value = 1;
    }

    return n;
error:
    return NULL;
}

/* void bst_n_delete    (const bst *T, bst_n *n)
 * void bst_n_delete_rec(const bst *T, bst_n *n)
 * Delete n, destroying stored data and freeing associated memory. No links are altered in
 * adjacent nodes, so don't call bst_n_delete on a node with children lest they become unreachable
 * in the void... use bst_n_delete_rec[ursively] to wipe out the whole subtree. */
void bst_n_delete(const bst *T, bst_n *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n);

    if (bst_n_has_key(n)) {
        t_destroy(T->key_type, bst_n_key(T, n));
    }
    if (bst_n_has_value(n)) {
        assert(T->value_type);
        t_destroy(T->value_type, bst_n_value(T, n));
    }
    free(n);
}

void bst_n_delete_rec(const bst *T, bst_n *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n);
    if (n->left) bst_n_delete_rec(T, n->left);
    if (n->right) bst_n_delete_rec(T, n->right);
    bst_n_delete(T, n);
}

/* bst_n *bst_n_find(const bst *T, const bst_n *n, const void *k)
 * Return the descendant node of n with the key k if present or NULL. */
bst_n *bst_n_find(const bst *T, bst_n *n, const void *k)
{
    assert(T && T->key_type && k);
    if (!n) return NULL;

    int cmp = t_compare(T->key_type, k, bst_n_key(T, n));
    if (cmp < 0) {
        return bst_n_find(T, n->left, k);
    } else if (cmp > 0) {
        return bst_n_find(T, n->right, k);
    } else { /* cmp == 0 */
        return n;
    }
}

/* int bst_n_insert(const bst *T, bst_n **np, const void *k, const void *v)
 * Insert a node with the key k and the value v (if given) into the substree with the root n.
 * The pointer at np may be changed. Return 1 if a node was added, 0 if k was already there, or -1
 * on failure. */
int bst_n_insert(bst *T, bst_n **np, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    bst_n *n = *np;

    if (!n) {
        n = bst_n_new(T, k, v);
        check(n, "failed to create new node");
        *np = n;
        return 1;
    }

    int cmp = t_compare(T->key_type, k, bst_n_key(T, n));

    if (cmp < 0) {
        return bst_n_insert(T, &n->left, k, v);
    } else if (cmp > 0) {
        return bst_n_insert(T, &n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bst_n_set_value(T, n, v);
        return 0;
    }

error:
    return -1;
}

/* int bst_n_remove_min(const bst *T, bst_n **np)
 * Remove the minimum from the subtree with the root n. The pointer at np may be changed. This
 * always removes a node, but we return 1 whatsoever for consistency with bst_n_remove. */
int bst_n_remove_min(bst *T, bst_n **np)
{
    bst_n *n = *np;
    assert(n);

    int rc;

    if (!n->left) {
        bst_n *r = n->right;
        bst_n_delete(T, n);
        *np = r;
        rc = 1;
    } else {
        rc = bst_n_remove_min(T, &n->left);
    }

    return rc;
}

/* void bst_n_move_data(const bst *T, bst_n *dest, bst_n *src)
 * Move the payload from src to dest, destroying any previous data in dest. */
void bst_n_move_data(const bst *T, bst_n *dest, bst_n *src)
{
    assert(T && T->key_type);
    assert(dest && src && bst_n_has_key(src));

    /* destroy the data in dest */
    if (bst_n_has_key(dest))     bst_n_destroy_key(T, dest);
    if (bst_n_has_value(dest))   bst_n_destroy_value(T, dest);

    /* move over the data from src */
    t_move(T->key_type, bst_n_key(T, dest), bst_n_key(T, src));
    src->flags.plain.has_key  = 0;
    dest->flags.plain.has_key = 1;

    if (bst_n_has_value(src)) {
        t_move(T->value_type, bst_n_value(T, dest), bst_n_value(T, src));
        src->flags.plain.has_value  = 0;
        dest->flags.plain.has_value = 1;
    }
}

/* int bst_n_remove(const bst *T, bst_n **np, const void *k)
 * Remove the node with the key k from the substree roted at n. The pointer at np may be changed.
 * Return 1 if a node was removed, 0 if k wasn't found, or -1 on error. */
int bst_n_remove(
        bst *T,
        bst_n **np,      /* the root of the substree to delete from */
        const void *k)  /* the key to delete */
{
    log_call("T=%p, np=%p, k=%p", T, np, k);
    assert(T && T->key_type && k);

    bst_n *n = *np;
    if (!n) return 0;

    int cmp = t_compare(T->key_type, k, bst_n_key(T, n));

    if      (cmp < 0) { return bst_n_remove(T, &n->left,  k); }
    else if (cmp > 0) { return bst_n_remove(T, &n->right, k); }
    else { /* cmp == 0 */
        if (n->left && n->right) {
            /* Find the node with the minimum key in the right subtree, which is guaranteed to not
             * have a left child; move its data over here, then delete it. */
            bst_n *s = n->right;
            while (s->left) s = s->left;
            bst_n_move_data(T, n, s);
            return bst_n_remove_min(T, &n->right);

        } else {
            if      (n->left)   *np = n->left;
            else if (n->right)  *np = n->right;
            else                *np = NULL;

            bst_n_delete(T, n);
            return 1;
        }
    }
}

/* void bst_n_set_key  (const bst *T, bst_n *n, const void *k)
 * void bst_n_set_value(const bst *T, bst_n *n, const void *v)
 * Set the key/value stored in n to k/v by copying it into the node. We assume that no previous
 * key/value is present. */
void bst_n_set_key(const bst *T, bst_n *n, const void *k)
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && n && k && T->key_type && !bst_n_has_key(n));
    t_copy(T->key_type, bst_n_key(T, n), k);
    n->flags.plain.has_key = 1;
}

void bst_n_set_value(const bst *T, bst_n *n, const void *v)
{
    log_call("T=%p, n=%p, v=%p", T, n, v);
    assert(T && n && v && T->value_type);
    if (bst_n_has_value(n)) bst_n_destroy_value(T, n);
    t_copy(T->value_type, bst_n_value(T, n), v);
    n->flags.plain.has_value = 1;
}

/* void bst_n_destroy_key  (const bst *T, bst_n *n)
 * void bst_n_destroy_value(const bst *T, bst_n *n, const void *v)
 * Destroy the key/value stored in n, freeing any associated memory. We assume that a key/value is
 * present. */
void bst_n_destroy_key(const bst *T, bst_n *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->key_type && bst_n_has_key(n));
    t_destroy(T->key_type, bst_n_key(T, n));
    n->flags.plain.has_key = 0;
}

void bst_n_destroy_value(const bst *T, bst_n *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->value_type && bst_n_has_value(n));
    t_destroy(T->value_type, bst_n_value(T, n));
    n->flags.plain.has_value = 0;
}

/* bst_n *bst_n_copy_rec(const bst *T, const bst_n *n)
 * Recursively copy the (sub)tree rooted at n, including all stored data. The new tree has the
 * exact same layout. */
bst_n *bst_n_copy_rec(const bst *T, const bst_n *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n && (!bst_n_has_value(n) || T->value_type));

    bst_n *c = bst_n_new(T, bst_n_key(T, n), bst_n_value(T, n));
    check(c != NULL, "failed to create new node");

    /* recursively copy the substrees */
    if (n->left) {
        c->left = bst_n_copy_rec(T, n->left);
    }
    if (n->right) {
        c->right = bst_n_copy_rec(T, n->right);
    }

    /* copy the complete flags byte */
    memcpy(&c->flags, &n->flags, sizeof(struct bst_n_flags));

    return c;
error:
    if (c) bst_n_delete_rec(T, c);
    return NULL;
}

/* int  bst_initialize(bst *T, uint8_t flavor, t_intf *kt, t_intf *vt)
 * bst *bst_new       (        uint8_t flavor, t_intf *kt, t_intf *vt)
 * bst_initialize initializes a bst at the address pointed to by T (assuming there's sufficient
 * space). bst_new allocates and initializes a new bst and returns a pointer to it. The type
 * interface for keys is required and must contain a at least a size and a comparison function.
 * The type interface for values can be NULL if the tree is going to store single elements. */
int bst_initialize(
        bst *T,             /* address of the bst to initialize */
        uint8_t flavor,     /* balancing strategy, one of NONE, RB, and AVL */
        t_intf *kt,         /* type interface for keys */
        t_intf *vt)         /* type interface for values, can be NULL */
{
    log_call("T=%p, flavor=%u, kt=%p, vt=%p", T, flavor, kt, vt);

    check_ptr(T);
    check(flavor <= 2, "bad flavor %u", flavor);
    check(kt != NULL, "no key type given");
    check(kt->compare != NULL, "key type but no comparison function");
    check(kt->size > 0, "size of 0 for keys?");
    check(!vt || vt->size > 0, "size of 0 for values?");

    T->root = NULL;
    T->count = 0;
    T->flavor = flavor;
    T->key_type = kt;
    T->value_type = vt;

    assert(bst_invariant(T, NULL) == 0);
    return 0;
error:
    return -1;
}

bst *bst_new(uint8_t flavor, t_intf *kt, t_intf *vt)
{
    log_call("flavor=%u, kt=%p, vt=%p", flavor, kt, vt);

    bst *T = calloc(1, sizeof(*T));
    check_alloc(T);

    int rc = bst_initialize(T, flavor, kt, vt);
    check_rc(rc, "bst_initialize");

    return T;
error:
    if (T) free(T);
    return NULL;
}

/* void bst_clear(bst *T)
 * Delete all nodes, freeing associated memory, and reset T. */
void bst_clear(bst *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bst_invariant(T, NULL) == 0);
        if (T->root) bst_n_delete_rec(T, T->root);
        T->root = NULL;
        T->count = 0;
    }
}

/* void bst_destroy(bst *T)
 * void bst_delete (bst *T)
 * Destroy T, freeing any associated memory. bst_delete also calls free on T. */
void bst_destroy(bst *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bst_invariant(T, NULL) == 0);
        if (T->root) bst_n_delete_rec(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bst_delete(bst *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bst_invariant(T, NULL) == 0);
        if (T->root) bst_n_delete_rec(T, T->root);
        free(T);
    }
}

/* bst *bst_copy   (           const bst *src)
 * int  bst_copy_to(bst *dest, const bst *src)
 * Copy a BST, duplicating all content and preserving the exact same layout. bst_copy makes the
 * copy on the heap, bst_copy_to creates it where dest points to. */

bst *bst_copy(const bst *src)
{
    log_call("src=%p", src);
    bst *dest = NULL;
    check_ptr(src);
    assert(bst_invariant(src, NULL) == 0);

    dest = bst_new(src->flavor, src->key_type, src->value_type);
    check(dest != NULL, "failed to create new tree");

    if (src->root) dest->root = bst_n_copy_rec(dest, src->root);
    dest->count = src->count;

    return dest;
error:
    if (dest) bst_delete(dest);
    return NULL;
}

int bst_copy_to(bst *dest, const bst *src)
{
    log_call("dest=%p, src=%p", dest, src);
    check_ptr(dest);
    check_ptr(src);
    assert(bst_invariant(src, NULL) == 0);

    int rc = bst_initialize(dest, src->flavor, src->key_type, src->value_type);
    check_rc(rc, "bst_initialize");

    if (src->root) dest->root = bst_n_copy_rec(dest, src->root);
    dest->count = src->count;

    return 0;
error:
    return -1;
}

/* int bst_has(const bst *T, const void *k)
 * Check if k is in T. */
int bst_has(const bst *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T, NULL) == 0);

    bst_n *n = T->root;
    int cmp;
    for ( ;; ) {
        if (!n) return 0;
        cmp = t_compare(T->key_type, k, bst_n_key(T, n));
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return 1; /* cmp == 0 */
    }

error:
    return -1;
}

/* int bst_insert(bst *T, const void *k)
 * Insert k into the tree, using the appropriate algorithm for the selected balancing strategy.
 * Return 1 if a node was added, 0 if k was already there, or -1 on error. */
int bst_insert(bst *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T, NULL) == 0);

    int rc;
    switch (T->flavor) {
        case RB:
            rc = rb_n_insert(T, &T->root, k, NULL);
            T->root->flags.rb.color = BLACK;
            break;
        case AVL:
            rc = avl_n_insert(T, &T->root, k, NULL, NULL);
            break;
        default:
            rc = bst_n_insert(T, &T->root, k, NULL);
    }

    if (rc == 1) ++T->count;
    assert(bst_invariant(T, NULL) == 0);
    return rc;
error:
    return -1;
}

/* int bst_remove(bst *T, const void *k)
 * Remove k from the tree, using the appropriate algorithm for the selected balancing strategy.
 * Return 1 if a node was deleted, 0 if k was not there, or -1 on error. */
int bst_remove(bst *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T, NULL) == 0);

    int rc;
    switch (T->flavor) {
        case RB:
            rc = rb_n_remove(T, &T->root, k);
            if (T->root) T->root->flags.rb.color = BLACK;
            break;
        case AVL:
            rc = avl_n_remove(T, &T->root, k, NULL);
            break;
        default:
            rc = bst_n_remove(T, &T->root, k);
    }

    if (rc == 1) --T->count;
    assert(bst_invariant(T, NULL) == 0);
    return rc;
error:
    return -1;
}

/* int bst_set(bst *T, const void *k, const void *v)
 * Set the value of the node with the key k to v, or insert a node with k and v if k doesn't
 * exist, using the appropriate algorithm for the selected balancing strategy. Return 1 if a node
 * was added, 0 if k was already there, or -1 on error. */
int bst_set(bst *T, const void *k, const void *v)
{
    log_call("T=%p, k=%p, v=%p", T, k, v);
    check_ptr(T);
    check_ptr(k);
    check_ptr(v);
    check(T->key_type, "no key type defined");
    check(T->value_type, "no value type defined");
    assert(bst_invariant(T, NULL) == 0);

    int rc;
    switch(T->flavor) {
        case RB:
            rc = rb_n_insert(T, &T->root, k, v);
            T->root->flags.rb.color = BLACK;
            break;
        case AVL:
            rc = avl_n_insert(T, &T->root, k, v, NULL);
            break;
        default:
            rc = bst_n_insert(T, &T->root, k, v);
    }

    if (rc == 1) ++T->count;
    assert(bst_invariant(T, NULL) == 0);
    return rc;
error:
    return -1;
}

/* void *bst_get(bst *T, const void *k)
 * Return a pointer to the value mapped to k in T or NULL if k doesn't exist. */
void *bst_get(bst *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    check(T->value_type, "no value type defined");
    assert(bst_invariant(T, NULL) == 0);

    bst_n *n = T->root;
    int cmp;
    while (n) {
        cmp = t_compare(T->key_type, k, bst_n_key(T, n));
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return bst_n_value(T, n); /* cmp == 0 */
    }

error: /* fallthrough */
    return NULL;
}

/* int bst_n_traverse             (        bst_n *n, int (*f)(bst_n *n, void *p), void *p)
 * int bst_n_traverse_r           (        bst_n *n, int (*f)(bst_n *n, void *p), void *p)
 * int bst_n_traverse_keys        (bst *T, bst_n *n, int (*f)(void *k, void *p), void *p)
 * int bst_n_traverse_keys_r      (bst *T, bst_n *n, int (*f)(void *k, void *p), void *p)
 * int bst_n_traverse_values      (bst *T, bst_n *n, int (*f)(void *v, void *p), void *p)
 * int bst_n_traverse_values_r    (bst *T, bst_n *n, int (*f)(void *v, void *p), void *p)
 *
 * Walk through all the nodes of the subtree with the root n in ascending/descending order. Call
 * f on every node, key, or value with the additional parameter p. If f returns a non-zero
 * integer, abort and return it.
 *
 * These functions are called by their counterparts bst_traverse... to do the actual work. There
 * should be no need to call them directly from the outside. */
int bst_n_traverse(
        bst_n *n,                        /* the root of the substree to traverse */
        int (*f)(bst_n *n, void *p),     /* the function to call on every node */
        void *p)                        /* additional parameter to pass to f */
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_n_traverse(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_n_traverse(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_n_traverse_r(bst_n *n, int (*f)(bst_n *n, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_n_traverse_r(n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_n_traverse_r(n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_n_traverse_keys(bst *T, bst_n *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_n_traverse_keys(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_n_key(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_n_traverse_keys(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_n_traverse_keys_r(bst *T, bst_n *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_n_traverse_keys_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_n_key(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_n_traverse_keys_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_n_traverse_values(bst *T, bst_n *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_n_traverse_values(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_n_value(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_n_traverse_values(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_n_traverse_values_r(bst *T, bst_n *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_n_traverse_values_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_n_value(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_n_traverse_values_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

/* int bst_traverse_nodes    (bst *T, int (*f)(bst_n *n,  void *p), void *p)
 * int bst_traverse_nodes_r  (bst *T, int (*f)(bst_n *n,  void *p), void *p)
 * int bst_traverse_keys     (bst *T, int (*f)(void *k, void *p), void *p)
 * int bst_traverse_keys_r   (bst *T, int (*f)(void *k, void *p), void *p)
 * int bst_traverse_values   (bst *T, int (*f)(void *v, void *p), void *p)
 * int bst_traverse_values_r (bst *T, int (*f)(void *v, void *p), void *p)
 *
 * Walk through all the nodes of the tree in ascending/descending order. Call f on every node,
 * key, or value with the additional parameter p. If f returns a non-zero integer, abort and
 * return it. */

int bst_traverse_nodes(bst *T, int (*f)(bst_n *n, void *p), void *p) {
    if (T && T->root) return bst_n_traverse(T->root, f, p);
    return 0;
}

int bst_traverse_nodes_r(bst *T, int (*f)(bst_n *n, void *p), void *p) {
    if (T && T->root) return bst_n_traverse_r(T->root, f, p);
    return 0;
}

int bst_traverse_keys(bst *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return bst_n_traverse_keys(T, T->root, f, p);
    return 0;
}

int bst_traverse_keys_r(bst *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return bst_n_traverse_keys_r(T, T->root, f, p);
    return 0;
}

int bst_traverse_values(bst *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return bst_n_traverse_values(T, T->root, f, p);
    return 0;
error:
    return -1;
}

int bst_traverse_values_r(bst *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return bst_n_traverse_values_r(T, T->root, f, p);
    return 0;
error:
    return -1;
}

/* size_t bst_n_height(const bst_n *n)
 * Get the height of the subtree with the root n, O(n)! */
size_t bst_n_height(const bst_n *n)
{
    if (!n) return 0;
    size_t hl = bst_n_height(n->left);
    size_t hr = bst_n_height(n->right);
    return (hl > hr ? hl : hr) + 1;
}

/* int bst_n_invariant(const bst *T, const bst_n *n, int depth, struct bst_stats *s)
 * Check if the subtree with the root n satisfies the inequality properties for keys in BSTs,
 * and collect stats of the tree while at it. */
int bst_n_invariant(const bst *T, const bst_n *n, int depth, struct bst_stats *s)
{
    if (!n) return 0;

    ++depth;
    ++s->total_nodes;

    if (!n->left && !n->right) {
        if (!s->shortest_path || depth < s->shortest_path) s->shortest_path = depth;
        if (!s->height        || depth > s->height)        s->height = depth;
    }

    /* check key inequalities */
    if (n->left && t_compare(T->key_type, bst_n_key(T, n->left), bst_n_key(T, n)) >= 0) {
        log_error("BST invariant violated: left child > parent");
        return -1;
    }
    if (n->right && t_compare(T->key_type, bst_n_key(T, n->right), bst_n_key(T, n)) <= 0) {
        log_error("BST invariant violated: right child < parent");
        return -1;
    }

    /* process children */
    if (n->left  && bst_n_invariant(T, n->left,  depth, s) != 0)   return -1;
    if (n->right && bst_n_invariant(T, n->right, depth, s) != 0)   return -1;

    return 0;
}

/* int bst_invariant(const bst *T, struct bst_stats *s_out)
 * Check if all pertinent invariants hold for the tree. If s_out is not NULL, save stats of the
 * tree there for further inspection. */
int bst_invariant(const bst *T, struct bst_stats *s_out)
{
    check_ptr(T);
    check(T->key_type, "malformed BST: no key type")

    struct bst_stats s = { 0 };
    int rc = 0;

    switch (T->flavor) {
        case RB:
            rc = rb_n_invariant(T, T->root, 0, 0, &s);
            break;
        case AVL:
            rc = avl_n_invariant(T, T->root, 0, NULL, &s);
            break;
        default:
            rc = bst_n_invariant(T, T->root, 0, &s);
    }

    check((int)T->count == s.total_nodes,
            "count (%u) and actual number of nodes (%d) differ", T->count, s.total_nodes);

    if (s_out) memcpy(s_out, &s, sizeof(s));
    return rc;
error:
    return -1;
}
