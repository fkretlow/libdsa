/*************************************************************************************************
 *
 * map.c
 *
 * The map functionality is almost entirely covered by the bst implementation. See bst.c and rb.c.
 *
 * Author: Florian Kretlow, 2021
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>

#include "check.h"
#include "map.h"

/* int map_remove(map *M, const void *k, void *out)
 * Remove k from the map. Move the value mapped to k to out if out is given. Return 1 if a node
 * was deleted, 0 if k wasn't found, or -1 on error. */
int map_remove(map *M, const void *k, void *out)
{
    check_ptr(M);
    check_ptr(k);

    bstn *n = bstn_find(M, M->root, k);
    if (!n) return 0;

    if (out) {
        assert(bstn_has_value(n));
        t_move(M->value_type, out, bstn_value(M, n));
        n->flags.plain.has_value = 0;
    }

    return bst_remove(M, k);
error:
    return -1;
}
