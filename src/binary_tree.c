#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "binary_tree.h"
#include "log.h"

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

    int rc = generate_memory_scheme(&T->scheme, kt, vt);
    check_rc(rc, "generate_memory_scheme");

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
