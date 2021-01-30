/*************************************************************************************************
 *
 * map.h
 *
 * Associative data structure that maps values to keys. Supports arbitrary data types by way of
 * type interface structs. This is just an adapter for the binary search tree, see bst.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _map_h
#define _map_h

#include "bst.h"

typedef bst map;

#define map_initialize(M, kt, vt)       bst_initialize(M, RB, kt, vt)
#define map_new(kt, vt)                 bst_new(RB, kt, vt)
#define map_destroy(M)                  bst_destroy(M)
#define map_delete(M)                   bst_delete(M)

#define map_clear(M)                    bst_clear(M)
#define map_copy(M)                     bst_copy(M)
#define map_copy_to(dest, src)          bst_copy_to(dest, src)

#define map_set(M, k, v)                bst_set(M, k, v)
#define map_get(M, k)                   bst_get(M, k)
#define map_has(M, k)                   bst_has(M, k)
#define map_remove(M, k)                bst_remove(M, k)

#define map_count(M)                    bst_count(M)

#endif /* _map_h */
