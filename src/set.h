/*************************************************************************************************
 *
 * set.h
 *
 * Declaration of the set container abstraction that stores unique values. Mostly an adapter to
 * the binary search tree, see bst.c for more info.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _set_h
#define _set_h

#include "bst.h"
#include "type_interface.h"

typedef bst set;

#define set_count(S) (S)->count

set *set_new(t_intf *dt);
#define set_delete(S)               bst_delete(S);
#define set_initialize(S, dt)       bst_initialize(S, RB, dt, NULL)
#define set_destroy(S)              bst_destroy(S)
#define set_clear(S)                bst_clear(S)
#define set_insert(S, e)            bst_insert(S, e)
#define set_remove(S, e)            bst_remove(S, e)
#define set_copy(S)                 bst_copy(S)
#define set_has(S, e)               bst_has(S, e);
#define set_traverse(S, f, p)       bst_traverse_keys(S, f, p)
#define set_traverse_r(S, f, p)     bst_traverse_keys_r(S, f, p)

set *set_union(set *S1, set *S2);
set *set_intersection(set *S1, set *S2);
set *set_difference(set *S1, set *S2);

#endif // _set_h
