#include <stdlib.h>
#include <string.h>

#include "binary_tree.h"

/***************************************************************************************
 *
 * int bst_initialize(bst *T, uint8_t flavor, t_intf *kt, t_intf *vt);
 *
 * Arguments:
 * bst *T           Address of the tree.
 * uint8_t flavor   Balancing strategy, one of NONE, RED_BLACK, and AVL.
 * t_intf *kt       Type interface for key objects.
 * t_intf *vt       Type interface for value objects. Can be NULL.
 *
 * Description:
 * Initializes a struct bst at the address pointed to by T. Sufficient space is assumed.
 * The type interface for keys is required and must contain a at least a size and a
 * comparison function. The type interface for values can be NULL if the tree is going
 * to store single elements.
 *
 * Return values:
 * 0 on success, -1 on error.
 *
 **************************************************************************************/

int bst_initialize(bst *T, uint8_t flavor, t_intf *kt, t_intf *vt)
{
    log_call("T=%p, flavor=%u, kt=%p, vt=%p", T, flavor, kt, vt);

    check(T != NULL, "T == NULL");
    check(0 <= flavor && flavor != 2, "flavor == %u", flavor);
    check(kt != NULL, "kt == NULL");
    check(kt->compare != NULL, "kt->compare == NULL");
    check(kt->size > 0, "kt->size == 0");
    check(!vt || vt->size > 0, "vt->size == 0");

    T->root = NULL;
    T->count = 0;
    T->flavor = flavor;

    int rc = generate_mem_scheme(&T->memory_scheme, kt, vt);
    check_rc(rc, "generate_mem_scheme");

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * bst *bst_new(uint8_t flavor, t_intf *kt, t_intf *vt);
 *
 * Arguments:
 * uint8_t flavor   Balancing strategy, one of NONE, RED_BLACK, and AVL.
 * t_intf *kt       Type interface for key objects.
 * t_intf *vt       Type interface for value objects. Can be NULL.
 *
 * Description:
 * Returns a pointer to a new bst that is allocated on the heap and initialized by
 * calling bst_initialize with the provided arguments (see there for further details).
 * It's the caller's responsibility to call bst_delete (or bst_destroy and free) on the
 * pointer to return the memory.
 *
 * Return values:
 * A pointer to an initialized bst on the heap or NULL on error.
 *
 **************************************************************************************/

bst *bst_new(uint8_t flavor, t_intf *kt, t_intf *vt);
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

/***************************************************************************************
 *
 * void bst_destroy(bst *T);
 * void bst_delete(bst *T);
 *
 * Destroy T, freeing any associated memory. bst_delete also calls free on T.
 *
 **************************************************************************************/

void bst_destroy(bst *T)
{
    if (T) {
        if (T->root) bstn_delete(T, T->root);
        memset(T, 0, sizeof(*T));
    }
}

void bst_delete(bst *T)
{
    if (T) {
        bst_destroy(T);
        free(T);
    }
}
