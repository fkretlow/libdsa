#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "binary_tree.h"
#include "log.h"

/***************************************************************************************
 *
 * btn *btn_new(const bt *T);
 *
 * Create a new node on the heap and return a pointer to it. Enough memory is requested
 * to store the node header, one key, and zero or one value objects according to the
 * type interfaces stored in T.
 *
 * Return values:
 * A pointer to the new node on success, NULL on error.
 *
 **************************************************************************************/

#define btn_size(T) \
    (sizeof(btn) + t_size((T)->key_type) \
                 + (T)->value_type ? t_size((T)->value_type) : 0)

btn *btn_new(const bt *T) {
    log_call("T=%p", T);
    check_ptr(T);
    check(T->key_type != NULL, "no key type");

    size_t size = btn_size(T);
    assert(size > sizeof(btn));
    btn *n = calloc(1, size);
    if (n == NULL) check_errno();
    return n;
error:
    return NULL;
}

/***************************************************************************************
 *
 * void btn_delete(const bt *T, btn *n);
 *
 * Delete n, destroying stored data and freeing associated memory. No node connections
 * are deleted: don't call btn_delete on a node with children lest they end up
 * unreachable, dangling in the void...
 *
 **************************************************************************************/

#define btn_has_key(n)   ((n)->flags.plain.has_key)
#define btn_has_value(n) ((n)->flags.plain.has_value)

#define btn_get_key(T, n)   (void*)(((char *)(n)) + sizeof(btn))
#define btn_get_value(T, n) (void*)(((char *)(n)) + sizeof(btn) + t_size((T)->key_type))

void btn_delete(const bt *T, btn *n)
{
    log_call("T=%p, n=%p");
    assert(n && T && T->key_type);

    if (btn_has_key(n)) {
        t_destroy(T->key_type, btn_get_key(T, n));
    }
    if (btn_has_value(n)) {
        t_destroy(T->value_type, btn_get_value(T, n));
    }
}

/***************************************************************************************
 *
 * void btn_set_key  (const bt *T, btn *n, const void *k);
 * void btn_set_value(const bt *T, btn *n, const void *v);
 *
 * Set the key/value stored in n to k/v by copying it into the node. We assume that no
 * previous key/value is present.
 *
 * void btn_destroy_key  (const bt *T, btn *n);
 * void btn_destroy_value(const bt *T, btn *n, const void *v);
 *
 * Destroy the key/value stored in n, freeing any associated memory. We assume that a
 * key/value is present.
 *
 **************************************************************************************/

void btn_set_key(const bt *T, btn *n, const void *k)
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && n && k && T->key_type && !btn_has_key(n));
    t_copy(T->key_type, btn_get_key(T, n), k);
}

void btn_set_value(const bt *T, btn *n, const void *v)
{
    log_call("T=%p, n=%p, v=%p", T, n, v);
    assert(T && n && v && T->value_type && !btn_has_value(n));
    t_copy(T->value_type, btn_get_value(T, n), v);
}

void btn_destroy_key(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->key_type && btn_has_key(n));
    t_destroy(T->key_type, btn_get_key(T, n));
}

void btn_destroy_value(const bt *T, btn *n)
{
    log_call("T=%p, n=%p", T, n);
    assert(T && n && T->value_type && btn_has_value(n));
    t_destroy(T->value_type, btn_get_value(T, n));
}

/***************************************************************************************
 *
 * int bt_initialize(bt *T, uint8_t flavor, t_intf *kt, t_intf *vt);
 *
 * Arguments:
 * bt *T            Address of the tree.
 * uint8_t flavor   Balancing strategy, one of NONE, RED_BLACK, and AVL.
 * t_intf *kt       Type interface for key objects.
 * t_intf *vt       Type interface for value objects. Can be NULL.
 *
 * Description:
 * Initializes a struct bt at the address pointed to by T. Sufficient space is assumed.
 * The type interface for keys is required and must contain a at least a size and a
 * comparison function. The type interface for values can be NULL if the tree is going
 * to store single elements.
 *
 * Return values:
 * 0 on success, -1 on error.
 *
 **************************************************************************************/

int bt_initialize(bt *T, uint8_t flavor, t_intf *kt, t_intf *vt)
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

/***************************************************************************************
 *
 * bt *bt_new(uint8_t flavor, t_intf *kt, t_intf *vt);
 *
 * Arguments:
 * uint8_t flavor   Balancing strategy, one of NONE, RED_BLACK, and AVL.
 * t_intf *kt       Type interface for key objects.
 * t_intf *vt       Type interface for value objects. Can be NULL.
 *
 * Description:
 * Returns a pointer to a new bt on the heap that is initialized by calling
 * bt_initialize with the provided arguments. It's the caller's responsibility to call
 * bt_delete (or bt_destroy and free) on the pointer to return the memory.
 *
 * Return values:
 * A pointer to an initialized bt on the heap or NULL on error.
 *
 **************************************************************************************/

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
 *
 * void bt_destroy(bt *T);
 * void bt_delete(bt *T);
 *
 * Destroy T, freeing any associated memory. bt_delete also calls free on T.
 *
 **************************************************************************************/

void bt_destroy(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        if (T->root) btn_delete(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bt_delete(bt *T)
{
    log_call("T=%p", T);
    if (T) {
        if (T->root) btn_delete(T, T->root);
        free(T);
    }
}
