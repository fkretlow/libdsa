#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "binary_tree.h"
#include "log.h"

/***************************************************************************************
 * btn *btn_new(const bt *T);
 * Create a new node on the heap and return a pointer to it, or NULL on error. Enough
 * memory is requested to store the node header, one key, and zero or one value objects
 * according to the type interfaces stored in T. */

#define btn_size(T) \
    (sizeof(btn) + t_size((T)->key_type) \
                 + ((T)->value_type ? t_size((T)->value_type) : 0))

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

/***************************************************************************************
 * void btn_delete    (const bt *T, btn *n);
 * void btn_delete_rec(const bt *T, btn *n);
 * Delete n, destroying stored data and freeing associated memory. No links are altered
 * in adjacent nodes. Don't call btn_delete on a node with children lest they become
 * unreachable in the void... use btn_delete_rec[ursively] to wipe out the whole
 * subtree. */

void btn_delete(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n);

    if (btn_has_key(n)) {
        t_destroy(T->key_type, btn_get_key(T, n));
    }
    if (btn_has_value(n)) {
        assert(T->value_type);
        t_destroy(T->value_type, btn_get_value(T, n));
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

/***************************************************************************************
 * void btn_set_key  (const bt *T, btn *n, const void *k);
 * void btn_set_value(const bt *T, btn *n, const void *v);
 * Set the key/value stored in n to k/v by copying it into the node. We assume that no
 * previous key/value is present. */

void btn_set_key(const bt *T, btn *n, const void *k)
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && n && k && T->key_type && !btn_has_key(n));
    t_copy(T->key_type, btn_get_key(T, n), k);
    n->flags.plain.has_key = 1;
}

void btn_set_value(const bt *T, btn *n, const void *v)
{
    log_call("T=%p, n=%p, v=%p", T, n, v);
    assert(T && n && v && T->value_type && !btn_has_value(n));
    t_copy(T->value_type, btn_get_value(T, n), v);
    n->flags.plain.has_value = 1;
}

/***************************************************************************************
 * void btn_destroy_key  (const bt *T, btn *n);
 * void btn_destroy_value(const bt *T, btn *n, const void *v);
 * Destroy the key/value stored in n, freeing any associated memory. We assume that a
 * key/value is present. */

void btn_destroy_key(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->key_type && btn_has_key(n));
    t_destroy(T->key_type, btn_get_key(T, n));
    n->flags.plain.has_key = 0;
}

void btn_destroy_value(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->value_type && btn_has_value(n));
    t_destroy(T->value_type, btn_get_value(T, n));
    n->flags.plain.has_value = 0;
}

/***************************************************************************************
 * btn *btn_copy_rec(const bt *T, const btn *n);
 * Recursively copy the (sub-)tree rooted at n, including all stored data. The new tree
 * has the exact same layout. */

btn *btn_copy_rec(const bt *T, const btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && T->key_type && n && (!btn_has_value(n) || T->value_type));

    btn *c = btn_new(T);
    check(c != NULL, "failed to create new node");

    /* copy the data */
    if (btn_has_key(n))     btn_set_key(T, c, btn_get_key(T, n));
    if (btn_has_value(n))   btn_set_value(T, c, btn_get_value(T, n));

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

/***************************************************************************************
 * void btn_rotate_left    (bt *T, btn *n, btn **n_out);
 * void btn_rotate_right   (bt *T, btn *n, btn **n_out);
 * void btn_replace_child  (bt *T, btn *p, btn *c, btn *s);
 * The usual tree rotations. */

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

void btn_replace_child(
        bt *T,  /* the tree, needed if c is the root */
        btn *p, /* the parent where a child should be replaced, NULL if c is the root */
        btn *c, /* the child to replace */
        btn *s) /* the new child (successor) */
{
    log_call("T=%p, p=%p, c=%p s=%p", T, p, c, s);
    assert(T && c && s);

    if (p == NULL) {
        assert(c == T->root);
        T->root = s;
        s->parent = NULL;
    } else {
        assert(c == p->left || c == p->right);
        if (c == p->left)  p->left = s;
        else               p->right = s;
        s->parent = p;
    }
}

/***************************************************************************************
 * int bt_initialize(bt *T, uint8_t flavor, t_intf *kt, t_intf *vt);
 * bt *bt_new       (       uint8_t flavor, t_intf *kt, t_intf *vt);
 * bt_initialize initializes a bt at the address pointed to by T (assuming there's
 * sufficient space). bt_new allocates and initializes a new bt and returns a pointer to
 * it. The type interface for keys is required and must contain a at least a size and a
 * comparison function. The type interface for values can be NULL if the tree is going
 * to store single elements. */

int bt_initialize(
        bt *T,          /* address of the bt to initialize */
        uint8_t flavor, /* balancing strategy, one of NONE, RED_BLACK, and AVL */
        t_intf *kt,     /* type interface for keys */
        t_intf *vt)     /* type interface for values, can be NULL */
{
    log_call("T=%p, flavor=%u, kt=%p, vt=%p", T, flavor, kt, vt);

    check(T != NULL, "T == NULL");
    check(flavor <= 2, "flavor == %u", flavor);
    check(kt != NULL, "kt == NULL");
    check(kt->compare != NULL, "kt->compare == NULL");
    check(kt->size > 0, "kt->size == 0");
    check(!vt || vt->size > 0, "vt->size == 0");

    T->root = NULL;
    T->count = 0;
    T->flavor = flavor;
    T->key_type = kt;
    T->value_type = vt;

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

/***************************************************************************************
 * void bt_destroy(bt *T);
 * void bt_delete (bt *T);
 * Destroy T, freeing any associated memory. bt_delete also calls free on T. */

void bt_destroy(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        if (T->root) btn_delete_rec(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bt_delete(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        if (T->root) btn_delete_rec(T, T->root);
        free(T);
    }
}

/***************************************************************************************
 * bt *bt_copy   (          const bt *src);
 * int bt_copy_to(bt *dest, const bt *src);
 * Copy a binary tree, duplicating all content and preserving the exact same layout.
 * bt_copy makes the copy on the heap, bt_copy_to creates it where dest points to. */

bt *bt_copy(const bt *src)
{
    log_call("src=%p", src);
    bt *dest = NULL;
    check_ptr(src);

    dest = bt_new(src->flavor, src->key_type, src->value_type);
    check(dest != NULL, "failed to create new tree");

    if (src->count > 0) dest->root = btn_copy_rec(dest, src->root);
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

    int rc = bt_initialize(dest, src->flavor, src->key_type, src->value_type);
    check_rc(rc, "bt_initialize");

    if (src->count > 0) dest->root = btn_copy_rec(dest, src->root);
    dest->count = src->count;

    return 0;
error:
    return -1;
}
