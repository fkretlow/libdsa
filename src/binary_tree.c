#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "binary_tree.h"
#include "log.h"

/*************************************************************************************************
 * btn *btn_new(const bt *T);
 * Create a new node on the heap and return a pointer to it, or NULL on error. Enough memory is
 * requested to store the node header, one key, and zero or one value objects according to the
 * type interfaces stored in T. */

#define btn_data_size(T) \
    (t_size((T)->key_type) + ((T)->value_type ? t_size((T)->value_type) : 0))
#define btn_size(T) (sizeof(btn) + btn_data_size(T))

btn *btn_new(const bt *T) {
    log_call("T=%p", T);
    assert(T && T->key_type);
    size_t size = btn_size(T);
    assert(size > sizeof(btn));

    btn *n = calloc(1, size);
    check_alloc(n);
    return n;
error:
    return NULL;
}

/*************************************************************************************************
 * void btn_delete    (const bt *T, btn *n);
 * void btn_delete_rec(const bt *T, btn *n);
 * Delete n, destroying stored data and freeing associated memory. No links are altered in
 * adjacent nodes. Don't call btn_delete on a node with children lest they become unreachable in
 * the void... use btn_delete_rec[ursively] to wipe out the whole subtree. */

void btn_delete(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n);

    if (btn_has_key(n)) {
        t_destroy(T->key_type, btn_key(T, n));
    }
    if (btn_has_value(n)) {
        assert(T->value_type);
        t_destroy(T->value_type, btn_value(T, n));
    }
    free(n);
}

void btn_delete_rec(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n);
    if (n->left) btn_delete_rec(T, n->left);
    if (n->right) btn_delete_rec(T, n->right);
    btn_delete(T, n);
}

/*************************************************************************************************
 * int btn_insert(const bt *T, btn *n, const void *k, btn **n_out);
 * Insert a node with the key k into the subtree rooted at n. A pointer to the new node is saved
 * in n_out. Returns 1 if a node was added, 0 if k was already there, and -1 on error. */

int btn_insert(
        bt *T,
        btn *n,         /* the root of the subtree, NULL if the tree is empty */
        const void *k,  /* the key to insert */
        btn **n_out)    /* pass back a pointer to the new node, can be NULL */
{
    log_call("T=%p, n=%p, k=%p, n_out=%p", T, n, k, n_out);
    assert(T && T->key_type && k);

    if (n == NULL) {
        assert(T->count == 0);
        T->root = btn_new(T);
        T->root->parent = NULL;
        btn_set_key(T, T->root, k);
        if (n_out) *n_out = T->root;
        return 1;
    }

    int comp = t_compare(T->key_type, k, btn_key(T, n));
    if (comp == 0) {
        if (n_out) *n_out = n;
        return 0; /* no new nodes */
    } else if (comp < 0 && n->left) {
        return btn_insert(T, n->left, k, n_out);
    } else if (comp > 0 && n->right) {
        return btn_insert(T, n->right, k, n_out);
    }

    else {
        if (comp < 0) {
            n->left = btn_new(T);
            n->left->parent = n;
            btn_set_key(T, n->left, k);
            if (n_out) *n_out = n->left;
        } else { /* comp > 0 */
            n->right = btn_new(T);
            n->right->parent = n;
            btn_set_key(T, n->right, k);
            if (n_out) *n_out = n->right;
        }

        return 1; /* one new node */
    }
}

/*************************************************************************************************
 * int btn_remove(const bt *T, btn *n, const void *k);
 * Remove the node with the key k from the subtree roted at n. Returns 1 if a node was deleted, 0
 * if k wasn't found, and -1 on error. */

int btn_remove(
        bt *T,
        btn *n,         /* the root of the subtree to delete from */
        const void *k)  /* the key to delete */
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && T->key_type && n && k);

    int comp;
    for ( ;; ) {
        if (!n) return 0;
        comp = t_compare(T->key_type, k, btn_key(T, n));
        if      (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
        else break;
    }

    if (n->left && n->right) {
        /* find the node with the next greater key and swap */
        btn *s = n->right;
        while (s->left) s = s->left;

        /* destroy the data in n */
        btn_destroy_key(T, n);
        if (btn_has_value(n)) btn_destroy_value(T, n);

        /* move over the data from s */
        t_move(T->key_type, btn_key(T, n), btn_key(T, s));
        s->flags.plain.has_key = 0;
        n->flags.plain.has_key = 1;

        if (btn_has_value(s)) {
            t_move(T->value_type, btn_value(T, n), btn_value(T, s));
            s->flags.plain.has_value = 0;
            n->flags.plain.has_value = 1;
        }

        /* move on, delete s, now stored in n */
        n = s;
    }

    if      (n->left)   btn_replace_child(T, n->parent, n, n->left);
    else if (n->right)  btn_replace_child(T, n->parent, n, n->right);
    else                btn_replace_child(T, n->parent, n, NULL);
    /* Now n is decoupled from adjacent nodes and btn_delete doesn't care about the links in n
     * itself. */
    btn_delete(T, n);
    return 1;
}

/*************************************************************************************************
 * void btn_set_key  (const bt *T, btn *n, const void *k);
 * void btn_set_value(const bt *T, btn *n, const void *v);
 * Set the key/value stored in n to k/v by copying it into the node. We assume that no previous
 * key/value is present. */

void btn_set_key(const bt *T, btn *n, const void *k)
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && n && k && T->key_type && !btn_has_key(n));
    t_copy(T->key_type, btn_key(T, n), k);
    n->flags.plain.has_key = 1;
}

void btn_set_value(const bt *T, btn *n, const void *v)
{
    log_call("T=%p, n=%p, v=%p", T, n, v);
    assert(T && n && v && T->value_type && !btn_has_value(n));
    t_copy(T->value_type, btn_value(T, n), v);
    n->flags.plain.has_value = 1;
}

/*************************************************************************************************
 * void btn_destroy_key  (const bt *T, btn *n);
 * void btn_destroy_value(const bt *T, btn *n, const void *v);
 * Destroy the key/value stored in n, freeing any associated memory. We assume that a
 * key/value is present. */

void btn_destroy_key(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type);
    assert(n);
    assert(btn_has_key(n));
    t_destroy(T->key_type, btn_key(T, n));
    n->flags.plain.has_key = 0;
}

void btn_destroy_value(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->value_type && btn_has_value(n));
    t_destroy(T->value_type, btn_value(T, n));
    n->flags.plain.has_value = 0;
}

/*************************************************************************************************
 * btn *btn_copy_rec(const bt *T, const btn *n);
 * Recursively copy the (sub-)tree rooted at n, including all stored data. The new tree has the
 * exact same layout. */

btn *btn_copy_rec(const bt *T, const btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n && (!btn_has_value(n) || T->value_type));

    btn *c = btn_new(T);
    check(c != NULL, "failed to create new node");

    /* copy the data */
    if (btn_has_key(n))     btn_set_key(T, c, btn_key(T, n));
    if (btn_has_value(n))   btn_set_value(T, c, btn_value(T, n));

    /* recursively copy the subtrees */
    if (n->left) {
        c->left = btn_copy_rec(T, n->left);
        c->left->parent = c;
    }
    if (n->right) {
        c->right = btn_copy_rec(T, n->right);
        c->right->parent = c;
    }

    /* copy the flags byte */
    memcpy(&c->flags, &n->flags, sizeof(struct btn_flags));

    return c;
error:
    if (c) btn_delete_rec(T, c);
    return NULL;
}

/*************************************************************************************************
 * void btn_rotate_left    (bt *T, btn *n, btn **n_out);
 * void btn_rotate_right   (bt *T, btn *n, btn **n_out);  The usual tree rotations. */

void btn_rotate_left(
        bt *T,          /* the tree, needed for btn_replace_child */
        btn *n,         /* the root of the subtree to rotate */
        btn **n_out)    /* write the address of the new root here, can be NULL */
{
    log_call("T=%p, n=%p, n_out=%p", T, n, n_out);
    assert(T && n);
    assert(n->right);   /* otherwise, why would we rotate? */

    btn *p = n->parent;
    btn *r = n->right;
    btn *rl = r->left;

    r->left = n; n->parent = r;
    n->right = rl;
    if (rl) rl->parent = n;
    btn_replace_child(T, p, n, r);

    if (n_out) *n_out = r;
}

void btn_rotate_right(bt *T, btn *n, btn **n_out)
{
    log_call("T=%p, n=%p, n_out=%p", T, n, n_out);
    assert(T && n);
    assert(n->right);

    btn *p = n->parent;
    btn *l = n->left;
    btn *lr = l->right;

    l->right = n; n->parent = l;
    n->left = lr;
    if (lr) lr->parent = n;
    btn_replace_child(T, p, n, l);

    if (n_out) *n_out = l;
}


/*************************************************************************************************
 * void btn_replace_child(bt *T, btn *p, btn *c, btn *s);
 * Replace the child c of p by a successor s. */

void btn_replace_child(
        bt *T,  /* the tree, needed if c is the root */
        btn *p, /* the parent where a child should be replaced, NULL if c is the root */
        btn *c, /* the child to replace */
        btn *s) /* the new child (successor), can be NULL */
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

/*************************************************************************************************
 * int bt_initialize(bt *T, uint8_t flavor, t_intf *kt, t_intf *vt);
 * bt *bt_new       (       uint8_t flavor, t_intf *kt, t_intf *vt);
 * bt_initialize initializes a bt at the address pointed to by T (assuming there's sufficient
 * space). bt_new allocates and initializes a new bt and returns a pointer to it. The type
 * interface for keys is required and must contain a at least a size and a comparison function.
 * The type interface for values can be NULL if the tree is going to store single elements. */

int bt_initialize(
        bt *T,              /* address of the bt to initialize */
        uint8_t flavor,     /* balancing strategy, one of NONE, RED_BLACK, and AVL */
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

    assert(bt_invariant(T) == 0);
    return 0;
error:
    return -1;
}

bt *bt_new(uint8_t flavor, t_intf *kt, t_intf *vt)
{
    log_call("flavor=%u, kt=%p, vt=%p", flavor, kt, vt);

    bt *T = calloc(1, sizeof(*T));
    check_alloc(T);

    int rc = bt_initialize(T, flavor, kt, vt);
    check_rc(rc, "bt_initialize");

    return T;
error:
    if (T) free(T);
    return NULL;
}

/*************************************************************************************************
 * void bt_clear(bt *T);
 * Delete all nodes, freeing associated memory, and reset T. */

void bt_clear(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bt_invariant(T) == 0);
        if (T->root) btn_delete_rec(T, T->root);
        T->root = NULL;
        T->count = 0;
    }
}

/*************************************************************************************************
 * void bt_destroy(bt *T);
 * void bt_delete (bt *T);
 * Destroy T, freeing any associated memory. bt_delete also calls free on T. */

void bt_destroy(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bt_invariant(T) == 0);
        if (T->root) btn_delete_rec(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bt_delete(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        assert(bt_invariant(T) == 0);
        if (T->root) btn_delete_rec(T, T->root);
        free(T);
    }
}

/*************************************************************************************************
 * bt *bt_copy   (          const bt *src);
 * int bt_copy_to(bt *dest, const bt *src);
 * Copy a binary tree, duplicating all content and preserving the exact same layout.  bt_copy
 * makes the copy on the heap, bt_copy_to creates it where dest points to. */

bt *bt_copy(const bt *src)
{
    log_call("src=%p", src);
    bt *dest = NULL;
    check_ptr(src);
    assert(bt_invariant(src) == 0);

    dest = bt_new(src->flavor, src->key_type, src->value_type);
    check(dest != NULL, "failed to create new tree");

    if (src->root) dest->root = btn_copy_rec(dest, src->root);
    dest->count = src->count;

    return dest;
error:
    if (dest) bt_delete(dest);
    return NULL;
}

int bt_copy_to(bt *dest, const bt *src)
{
    log_call("dest=%p, src=%p", dest, src);
    check_ptr(dest);
    check_ptr(src);
    assert(bt_invariant(src) == 0);

    int rc = bt_initialize(dest, src->flavor, src->key_type, src->value_type);
    check_rc(rc, "bt_initialize");

    if (src->root) dest->root = btn_copy_rec(dest, src->root);
    dest->count = src->count;

    return 0;
error:
    return -1;
}

/*************************************************************************************************
 * int bt_has(const bt *T, const void *k); Check if k is in T. */

int bt_has(const bt *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bt_invariant(T) == 0);

    btn *n = T->root;
    int comp;
    for ( ;; ) {
        if (!n) return 0;
        comp = t_compare(T->key_type, k, btn_key(T, n));
        if (comp == 0) return 1;
        else if (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
    }

error:
    return -1;
}


/*************************************************************************************************
 * int bt_insert(bt *T, const void *k);
 * Insert k into the tree. Return 0 or 1 depending on whether a node was added or k was
 * already there, or -1 on error. */

int bt_insert(bt *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bt_invariant(T) == 0);

    int rc = btn_insert(T, T->root, k, NULL);
    if (rc == 1) ++T->count;

    assert(bt_invariant(T) == 0);
    return rc;
error:
    return -1;
}

/*************************************************************************************************
 * int bt_remove(bt *T, const void *k);
 * Remove k from the tree. Return 0 or 1 depending on whether k was found and removed or not, or
 * -1 on error. */

int bt_remove(bt *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(bt_invariant(T) == 0);

    int rc = (T->root) ? btn_remove(T, T->root, k) : 0;
    if (rc == 1) --T->count;

    assert(bt_invariant(T) == 0);
    return rc;
error:
    return -1;
}

/*************************************************************************************************
 * int bt_set(bt *T, const void *k, const void *v)
 * Set the value of the node with the key k to v, or insert a node with k and v if k doesn't
 * exsit. Return 0 or 1 depending on whether a node was added or k was already there, or -1 on
 * error. */

int bt_set(bt *T, const void *k, const void *v)
{
    log_call("T=%p, k=%p, v=%p", T, k, v);
    check_ptr(T);
    check_ptr(k);
    check_ptr(v);
    check(T->key_type, "no key type defined");
    check(T->value_type, "no value type defined");
    assert(bt_invariant(T) == 0);

    btn *n = NULL;
    int rc = btn_insert(T, T->root, k, &n);
    if (rc == 1) ++T->count;

    if (btn_has_value(n)) btn_destroy_value(T, n);
    btn_set_value(T, n, v);

    assert(bt_invariant(T) == 0);
    return rc;
error:
    return -1;
}

/*************************************************************************************************
 * void *bt_get(bt *T, const void *k);
 * Return a pointer to the value mapped to k in T or NULL if k doesn't exist. */

void *bt_get(bt *T, const void *k)
{
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    check(T->value_type, "no value type defined");
    assert(bt_invariant(T) == 0);

    btn *n = T->root;
    int comp;
    for ( ;; ) {
        if (!n) return NULL;
        comp = t_compare(T->key_type, k, btn_key(T, n));
        if (comp == 0) return btn_value(T, n);
        else if (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
    }

error:
    return NULL;

}

/*************************************************************************************************
 * int btn_traverse             (       btn *n, int (*f)(btn *n,  void *p), void *p);
 * int btn_traverse_r           (       btn *n, int (*f)(btn *n,  void *p), void *p);
 * int btn_traverse_keys        (bt *T, btn *n, int (*f)(void *k, void *p), void *p);
 * int btn_traverse_keys_r      (bt *T, btn *n, int (*f)(void *k, void *p), void *p);
 * int btn_traverse_values      (bt *T, btn *n, int (*f)(void *v, void *p), void *p);
 * int btn_traverse_values_r    (bt *T, btn *n, int (*f)(void *v, void *p), void *p);
 *
 * Walk through all the nodes of the sub-tree with the root n in ascending/descending order. Call
 * f on every node, key, or value with the additional parameter p. If f returns a non-zero
 * integer, abort and return it.
 *
 * These functions are called by their counterparts bt_traverse... to do the actual work. There
 * should be no need to call them directly from the outside. */

int btn_traverse(
        btn *n,                         /* the root of the subtree to traverse */
        int (*f)(btn *n, void *p),      /* the function to call on every node */
        void *p)                        /* additional parameter to pass to f */
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = btn_traverse(n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = btn_traverse(n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int btn_traverse_r(btn *n, int (*f)(btn *n, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = btn_traverse_r(n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(n, p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = btn_traverse_r(n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int btn_traverse_keys(bt *T, btn *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = btn_traverse_keys(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(btn_key(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = btn_traverse_keys(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int btn_traverse_keys_r(bt *T, btn *n, int (*f)(void *k, void *p), void *p)
{
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = btn_traverse_keys_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(btn_key(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = btn_traverse_keys_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int btn_traverse_values(bt *T, btn *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->left) {
            rc = btn_traverse_values(T, n->left, f, p);
            if (rc != 0) return rc;
        }

        rc = f(btn_value(T, n), p);
        if (rc != 0) return rc;

        if (n->right) {
            rc = btn_traverse_values(T, n->right, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

int btn_traverse_values_r(bt *T, btn *n, int (*f)(void *v, void *p), void *p)
{
    assert(T->value_type);
    int rc = 0;

    if (n) {
        if (n->right) {
            rc = btn_traverse_values_r(T, n->right, f, p);
            if (rc != 0) return rc;
        }

        rc = f(btn_value(T, n), p);
        if (rc != 0) return rc;

        if (n->left) {
            rc = btn_traverse_values_r(T, n->left, f, p);
            if (rc != 0) return rc;
        }
    }

    return rc;
}

/*************************************************************************************************
 * int bt_traverse_nodes    (bt *T, int (*f)(btn *n,  void *p), void *p);
 * int bt_traverse_nodes_r  (bt *T, int (*f)(btn *n,  void *p), void *p);
 * int bt_traverse_keys     (bt *T, int (*f)(void *k, void *p), void *p);
 * int bt_traverse_keys_r   (bt *T, int (*f)(void *k, void *p), void *p);
 * int bt_traverse_values   (bt *T, int (*f)(void *v, void *p), void *p);
 * int bt_traverse_values_r (bt *T, int (*f)(void *v, void *p), void *p);
 *
 * Walk through all the nodes of the tree in ascending/descending order. Call f on every node,
 * key, or value with the additional parameter p. If f returns a non-zero integer, abort and
 * return it. */

int bt_traverse_nodes(bt *T, int (*f)(btn *n, void *p), void *p) {
    if (T && T->root) return btn_traverse(T->root, f, p);
    return 0;
}

int bt_traverse_nodes_r(bt *T, int (*f)(btn *n, void *p), void *p) {
    if (T && T->root) return btn_traverse_r(T->root, f, p);
    return 0;
}

int bt_traverse_keys(bt *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return btn_traverse_keys(T, T->root, f, p);
    return 0;
}

int bt_traverse_keys_r(bt *T, int (*f)(void *k, void *p), void *p) {
    if (T && T->root) return btn_traverse_keys_r(T, T->root, f, p);
    return 0;
}

int bt_traverse_values(bt *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return btn_traverse_values(T, T->root, f, p);
    return 0;
error:
    return -1;
}

int bt_traverse_values_r(bt *T, int (*f)(void *v, void *p), void *p) {
    check(T->value_type, "the tree doesn't store values");
    if (T && T->root) return btn_traverse_values_r(T, T->root, f, p);
    return 0;
error:
    return -1;
}

/*************************************************************************************************
 * int btn_invariant(const bt *T, const btn *n)
 * int bt_invariant(bt *T)
 * Check if T satisfies the inequality properties for keys in binary trees. */

int btn_invariant(const bt *T, const btn *n)
{
    if (n) {
        if (n->left && btn_invariant(T, n->left) != 0) return -1;

        if (n->parent) {
            if (n == n->parent->left) {
                check(t_compare(T->key_type, btn_key(T, n), btn_key(T, n->parent)) <= 0,
                        "binary tree invariant violated: left child > parent");
            } else { /* n == n->parent->right */
                check(t_compare(T->key_type, btn_key(T, n), btn_key(T, n->parent)) >= 0,
                        "binary tree invariant violated: right child < parent");
            }
        }

        if (n->right && btn_invariant(T, n->right) != 0) return -1;
    }

    return 0;
error:
    return -1;
}

int bt_invariant(const bt *T)
{
    if (T) {
        if (T->count > 0) {
            check(T->root != NULL, "binary tree invariant violated: non-zero count but no root");
            return btn_invariant(T, T->root);
        }
    }
    return 0;
error:
    return -1;
}
