#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "bst.h"
#include "log.h"


/* bst_node *bst_node_new(const bst *T, const void *k, const void *v)
 * Create a new node with the key k and the value v (if given) on the heap and return a pointer to
 * it, or NULL on error. Enough memory is requested to store the node header, one key, and zero or
 * one value objects according to the type interfaces stored in T. */

bst_node *bst_node_new(const bst *T, const void *k, const void *v)
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    size_t size = bst_node_size(T);
    bst_node *n = calloc(1, size);
    check_alloc(n);

    t_copy(T->key_type, bst_node_key(T, n), k);
    n->flags.plain.has_key = 1;
    if (v) {
        t_copy(T->value_type, bst_node_value(T, n), v);
        n->flags.plain.has_value = 1;
    }

    if (T->flavor == RB) n->flags.rb.color = RED;

    return n;
error:
    return NULL;
}


/* void bst_node_delete    (const bst *T, bst_node *n)
 * void bst_node_delete_rec(const bst *T, bst_node *n)
 * Delete n, destroying stored data and freeing associated memory. No links are altered in
 * adjacent nodes. Don't call bst_node_delete on a node with children lest they become unreachable in
 * the void... use bst_node_delete_rec[ursively] to wipe out the whole substree. */

void bst_node_delete(const bst *T, bst_node *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n);

    if (bst_node_has_key(n)) {
        t_destroy(T->key_type, bst_node_key(T, n));
    }
    if (bst_node_has_value(n)) {
        assert(T->value_type);
        t_destroy(T->value_type, bst_node_value(T, n));
    }
    free(n);
}

void bst_node_delete_rec(const bst *T, bst_node *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n);
    if (n->left) bst_node_delete_rec(T, n->left);
    if (n->right) bst_node_delete_rec(T, n->right);
    bst_node_delete(T, n);
}


/* bst_node *bst_node_insert(const bst *T, bst_node *n, const void *k, const void *v)
 * Insert a node with the key k and the value v (if given) into the substree rooted at n, return
 * n. */

bst_node *bst_node_insert(bst *T, bst_node *n, const void *k, const void *v)
{
    if (!n) {
        ++T->count;
        return bst_node_new(T, k, v);
    }

    int cmp = t_compare(T->key_type, k, bst_node_key(T, n));

    if (cmp < 0) {
        n->left = bst_node_insert(T, n->left, k, v);
    } else if (cmp > 0) {
        n->right = bst_node_insert(T, n->right, k, v);
    } else { /* cmp == 0 */
        if (v) bst_node_set_value(T, n, v);
    }

    return n;
}

/* bst_node *bst_node_remove_min(const bst *T, bst_node *n)
 * bst_node *bst_node_remove(const bst *T, bst_node *n, const void *k)
 * Remove the node with the key k from the substree roted at n, return n. */

bst_node *bst_node_remove_min(bst *T, bst_node *n)
{
    if (n->left) {
        n->left = bst_node_remove_min(T, n->left);
        return n;
    } else {
        bst_node *r = n->right;
        bst_node_delete(T, n);
        --T->count;
        return r;
    }
}

bst_node *bst_node_remove(
        bst *T,
        bst_node *n,        /* the root of the substree to delete from */
        const void *k)  /* the key to delete */
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && T->key_type && k);

    if (!n) return n;

    int cmp = t_compare(T->key_type, k, bst_node_key(T, n));

    if (cmp < 0) {
        n->left = bst_node_remove(T, n->left, k);
        return n;
    } else if (cmp > 0) {
        n->right = bst_node_remove(T, n->right, k);
        return n;
    } else { /* cmp == 0 */
        bst_node *s;
        if (n->left && n->right) {
            /* find the node with the next greater key and swap */
            s = n->right;
            while (s->left) s = s->left;

            /* destroy the data in n */
            bst_node_destroy_key(T, n);
            if (bst_node_has_value(n)) bst_node_destroy_value(T, n);

            /* move over the data from s */
            t_move(T->key_type, bst_node_key(T, n), bst_node_key(T, s));
            s->flags.plain.has_key = 0;
            n->flags.plain.has_key = 1;

            if (bst_node_has_value(s)) {
                t_move(T->value_type, bst_node_value(T, n), bst_node_value(T, s));
                s->flags.plain.has_value = 0;
                n->flags.plain.has_value = 1;
            }

            /* move on, delete s */
            n->right = bst_node_remove_min(T, n->right);
            return n;

        } else if (n->left) {
            s = n->left;
            bst_node_delete(T, n);
            --T->count;
            return s;
        } else if (n->right) {
            s = n->right;
            bst_node_delete(T, n);
            --T->count;
            return s;
        } else {
            bst_node_delete(T, n);
            --T->count;
            return NULL;
        }
    }
}

/* void bst_node_set_key  (const bst *T, bst_node *n, const void *k)
 * void bst_node_set_value(const bst *T, bst_node *n, const void *v)
 * Set the key/value stored in n to k/v by copying it into the node. We assume that no previous
 * key/value is present. */

void bst_node_set_key(const bst *T, bst_node *n, const void *k)
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && n && k && T->key_type && !bst_node_has_key(n));
    t_copy(T->key_type, bst_node_key(T, n), k);
    n->flags.plain.has_key = 1;
}

void bst_node_set_value(const bst *T, bst_node *n, const void *v)
{
    log_call("T=%p, n=%p, v=%p", T, n, v);
    assert(T && n && v && T->value_type);
    if (bst_node_has_value(n)) bst_node_destroy_value(T, n);
    t_copy(T->value_type, bst_node_value(T, n), v);
    n->flags.plain.has_value = 1;
}


/* void bst_node_destroy_key  (const bst *T, bst_node *n)
 * void bst_node_destroy_value(const bst *T, bst_node *n, const void *v)
 * Destroy the key/value stored in n, freeing any associated memory. We assume that a
 * key/value is present. */

void bst_node_destroy_key(const bst *T, bst_node *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type);
    assert(n);
    assert(bst_node_has_key(n));
    t_destroy(T->key_type, bst_node_key(T, n));
    n->flags.plain.has_key = 0;
}

void bst_node_destroy_value(const bst *T, bst_node *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->value_type && bst_node_has_value(n));
    t_destroy(T->value_type, bst_node_value(T, n));
    n->flags.plain.has_value = 0;
}


/* bst_node *bst_node_copy_rec(const bst *T, const bst_node *n)
 * Recursively copy the (sub-)tree rooted at n, including all stored data. The new tree has the
 * exact same layout. */

bst_node *bst_node_copy_rec(const bst *T, const bst_node *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n && (!bst_node_has_value(n) || T->value_type));

    bst_node *c = bst_node_new(T, bst_node_key(T, n), bst_node_value(T, n));
    check(c != NULL, "failed to create new node");

    /* recursively copy the substrees */
    if (n->left) {
        c->left = bst_node_copy_rec(T, n->left);
    }
    if (n->right) {
        c->right = bst_node_copy_rec(T, n->right);
    }

    /* copy the complete flags byte */
    memcpy(&c->flags, &n->flags, sizeof(struct bst_node_flags));

    return c;
error:
    if (c) bst_node_delete_rec(T, c);
    return NULL;
}


/* void bst_node_rotate_left    (bst *T, bst_node *n, bst_node **n_out)
 * void bst_node_rotate_right   (bst *T, bst_node *n, bst_node **n_out)  The usual tree rotations. */

void bst_node_rotate_left(
        bst *T,          /* the tree, needed for bst_node_replace_child */
        bst_node *n,         /* the root of the substree to rotate */
        bst_node **n_out)    /* write the address of the new root here, can be NULL */
{
    log_call("T=%p, n=%p, n_out=%p", T, n, n_out);
    assert(T && n);
    assert(n->right);   /* otherwise, why would we rotate? */

    bst_node *p = n->parent;
    bst_node *r = n->right;
    bst_node *rl = r->left;

    r->left = n; n->parent = r;
    n->right = rl;
    if (rl) rl->parent = n;
    bst_node_replace_child(T, p, n, r);

    if (n_out) *n_out = r;
}

void bst_node_rotate_right(bst *T, bst_node *n, bst_node **n_out)
{
    log_call("T=%p, n=%p, n_out=%p", T, n, n_out);
    assert(T && n);
    assert(n->left);

    bst_node *p = n->parent;
    bst_node *l = n->left;
    bst_node *lr = l->right;

    l->right = n; n->parent = l;
    n->left = lr;
    if (lr) lr->parent = n;
    bst_node_replace_child(T, p, n, l);

    if (n_out) *n_out = l;
}


/* void bst_node_replace_child(bst *T, bst_node *p, bst_node *c, bst_node *s)
 * Replace the child c of p by a successor s. */

void bst_node_replace_child(
        bst *T,  /* the tree, needed if c is the root */
        bst_node *p, /* the parent where a child should be replaced, NULL if c is the root */
        bst_node *c, /* the child to replace */
        bst_node *s) /* the new child (successor), can be NULL */
{
    log_call("T=%p, p=%p, c=%p s=%p", T, p, c, s);
    assert(T && c);

    if (p == NULL) {
        assert(c == T->root);
        T->root = s;
        if (s) s->parent = NULL;
    } else {
        assert(c == p->left || c == p->right);
        if (c == p->left)  p->left = s;
        else               p->right = s;
        if (s) s->parent = p;
    }
}


/* int bst_initialize(bst *T, uint8_t flavor, t_intf *kt, t_intf *vt)
 * bst *bst_new      (        uint8_t flavor, t_intf *kt, t_intf *vt)
 * bst_initialize initializes a bst at the address pointed to by T (assuming there's sufficient
 * space). bst_new allocates and initializes a new bst and returns a pointer to it. The type
 * interface for keys is required and must contain a at least a size and a comparison function.
 * The type interface for values can be NULL if the tree is going to store single elements. */

int bst_initialize(
        bst *T,             /* address of the bst to initialize */
        uint8_t flavor,     /* balancing strategy, one of NONE, RB,, and AVL */
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

    assert(bst_invariant(T) == 0);
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
        assert(bst_invariant(T) == 0);
        if (T->root) bst_node_delete_rec(T, T->root);
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
        assert(bst_invariant(T) == 0);
        if (T->root) bst_node_delete_rec(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bst_delete(bst *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bst_invariant(T) == 0);
        if (T->root) bst_node_delete_rec(T, T->root);
        free(T);
    }
}


/* bst *bst_copy   (          const bst *src)
 * int bst_copy_to(bst *dest, const bst *src)
 * Copy a binary tree, duplicating all content and preserving the exact same layout.  bst_copy
 * makes the copy on the heap, bst_copy_to creates it where dest points to. */

bst *bst_copy(const bst *src)
{
    log_call("src=%p", src);
    bst *dest = NULL;
    check_ptr(src);
    assert(bst_invariant(src) == 0);

    dest = bst_new(src->flavor, src->key_type, src->value_type);
    check(dest != NULL, "failed to create new tree");

    if (src->root) dest->root = bst_node_copy_rec(dest, src->root);
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
    assert(bst_invariant(src) == 0);

    int rc = bst_initialize(dest, src->flavor, src->key_type, src->value_type);
    check_rc(rc, "bst_initialize");

    if (src->root) dest->root = bst_node_copy_rec(dest, src->root);
    dest->count = src->count;

    return 0;
error:
    return -1;
}


/* int bst_has(const bst *T, const void *k) Check if k is in T. */

int bst_has(const bst *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T) == 0);

    bst_node *n = T->root;
    int cmp;
    for ( ;; ) {
        if (!n) return 0;
        cmp = t_compare(T->key_type, k, bst_node_key(T, n));
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return 1; /* cmp == 0 */
    }

error:
    return -1;
}

/* int bst_insert(bst *T, const void *k)
 * Insert k into the tree. Return 0 or 1 depending on whether a node was added or k was
 * already there, or -1 on error. */

int bst_insert(bst *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T) == 0);

    switch (T->flavor) {
        case RB:
            T->root = rb_node_insert(T, T->root, k, NULL);
            assert(bst_invariant(T) == 0);
        default:
            T->root = bst_node_insert(T, T->root, k, NULL);
            assert(bst_invariant(T) == 0);
    }

    return 0;
error:
    return -1;
}


/* int bst_remove(bst *T, const void *k)
 * Remove k from the tree. Return 0 or 1 depending on whether k was found and removed or not, or
 * -1 on error. */

int bst_remove(bst *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bst_invariant(T) == 0);

    switch (T->flavor) {
        default:
            T->root = bst_node_remove(T, T->root, k);
            assert(bst_invariant(T) == 0);
    }

    return 0;
error:
    return -1;
}


/* int bst_set(bst *T, const void *k, const void *v)
 * Set the value of the node with the key k to v, or insert a node with k and v if k doesn't
 * exist. Return 0 or 1 depending on whether a node was added or k was already there, or -1 on
 * error. */

int bst_set(bst *T, const void *k, const void *v)
{
    log_call("T=%p, k=%p, v=%p", T, k, v);
    check_ptr(T);
    check_ptr(k);
    check_ptr(v);
    check(T->key_type, "no key type defined");
    check(T->value_type, "no value type defined");
    assert(bst_invariant(T) == 0);

    switch(T->flavor) {
        case RB:
            T->root = rb_node_insert(T, T->root, k, NULL);
            assert(bst_invariant(T) == 0);
        default:
            T->root = bst_node_insert(T, T->root, k, v);
            assert(bst_invariant(T) == 0);
    }

    return 0;
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
    assert(bst_invariant(T) == 0);

    bst_node *n = T->root;
    int cmp;
    for ( ;; ) {
        if (!n) return NULL;
        cmp = t_compare(T->key_type, k, bst_node_key(T, n));
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return bst_node_value(T, n); /* cmp == 0 */
    }

error:
    return NULL;
}


/* int bst_node_traverse             (       bst_node *n, int (*f)(bst_node *n,  void *p), void *p)
 * int bst_node_traverse_r           (       bst_node *n, int (*f)(bst_node *n,  void *p), void *p)
 * int bst_node_traverse_keys        (bst *T, bst_node *n, int (*f)(void *k, void *p), void *p)
 * int bst_node_traverse_keys_r      (bst *T, bst_node *n, int (*f)(void *k, void *p), void *p)
 * int bst_node_traverse_values      (bst *T, bst_node *n, int (*f)(void *v, void *p), void *p)
 * int bst_node_traverse_values_r    (bst *T, bst_node *n, int (*f)(void *v, void *p), void *p)
 *
 * Walk through all the nodes of the sub-tree with the root n in ascending/descending order. Call
 * f on every node, key, or value with the additional parameter p. If f returns a non-zero
 * integer, abort and return it.
 *
 * These functions are called by their counterparts bst_traverse... to do the actual work. There
 * should be no need to call them directly from the outside. */

int bst_node_traverse(
        bst_node *n,                         /* the root of the substree to traverse */
        int (*f)(bst_node *n, void *p),      /* the function to call on every node */
        void *p)                        /* additional parameter to pass to f */
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_node_traverse(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_node_traverse(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_node_traverse_r(bst_node *n, int (*f)(bst_node *n, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_node_traverse_r(n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_node_traverse_r(n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_node_traverse_keys(bst *T, bst_node *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_node_traverse_keys(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_node_key(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_node_traverse_keys(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_node_traverse_keys_r(bst *T, bst_node *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_node_traverse_keys_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_node_key(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_node_traverse_keys_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_node_traverse_values(bst *T, bst_node *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = bst_node_traverse_values(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_node_value(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = bst_node_traverse_values(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int bst_node_traverse_values_r(bst *T, bst_node *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = bst_node_traverse_values_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(bst_node_value(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = bst_node_traverse_values_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}


/* int bst_traverse_nodes    (bst *T, int (*f)(bst_node *n,  void *p), void *p)
 * int bst_traverse_nodes_r  (bst *T, int (*f)(bst_node *n,  void *p), void *p)
 * int bst_traverse_keys     (bst *T, int (*f)(void *k, void *p), void *p)
 * int bst_traverse_keys_r   (bst *T, int (*f)(void *k, void *p), void *p)
 * int bst_traverse_values   (bst *T, int (*f)(void *v, void *p), void *p)
 * int bst_traverse_values_r (bst *T, int (*f)(void *v, void *p), void *p)
 *
 * Walk through all the nodes of the tree in ascending/descending order. Call f on every node,
 * key, or value with the additional parameter p. If f returns a non-zero integer, abort and
 * return it. */

int bst_traverse_nodes(bst *T, int (*f)(bst_node *n, void *p), void *p) {
    if (T && T->root) return bst_node_traverse(T->root, f, p);
    return 0;
}

int bst_traverse_nodes_r(bst *T, int (*f)(bst_node *n, void *p), void *p) {
    if (T && T->root) return bst_node_traverse_r(T->root, f, p);
    return 0;
}

int bst_traverse_keys(bst *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return bst_node_traverse_keys(T, T->root, f, p);
    return 0;
}

int bst_traverse_keys_r(bst *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return bst_node_traverse_keys_r(T, T->root, f, p);
    return 0;
}

int bst_traverse_values(bst *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return bst_node_traverse_values(T, T->root, f, p);
    return 0;
error:
    return -1;
}

int bst_traverse_values_r(bst *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return bst_node_traverse_values_r(T, T->root, f, p);
    return 0;
error:
    return -1;
}


/* int bst_node_invariant(const bst *T, const bst_node *n)
 * int bst_invariant(bst *T)
 * Check if T satisfies the inequality properties for keys in binary trees. */

int bst_node_invariant(const bst *T, const bst_node *n)
{
    if (n) {
        if (n->left && bst_node_invariant(T, n->left) != 0) return -1;

        if (n->left)
            check(t_compare(T->key_type, bst_node_key(T, n->left), bst_node_key(T, n)) <= 0,
                    "binary tree invariant violated: left child > parent");
        if (n->right)
            check(t_compare(T->key_type, bst_node_key(T, n->right), bst_node_key(T, n)) >= 0,
                    "binary tree invariant violated: right child < parent");

        if (n->right && bst_node_invariant(T, n->right) != 0) return -1;
    }

    return 0;
error:
    return -1;
}

int bst_invariant(const bst *T)
{
    if (T) {
        if (T->count > 0) {
            check(T->root != NULL, "binary tree invariant violated: non-zero count but no root");
            return bst_node_invariant(T, T->root);
        }
    }
    return 0;
error:
    return -1;
}
