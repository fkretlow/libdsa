#ifndef _set_h
#define _set_h

#include "binary_tree.h"
#include "type_interface.h"

typedef bt set;

#define set_count(S) (S)->count

set *set_new(t_intf *dt);
void set_delete(set *S);

#define set_initialize(S, dt)       bt_initialize(S, RED_BLACK, dt, NULL)
#define set_clear(S)                bt_clear(S)
#define set_insert(S, e)            bt_insert(S, e)
#define set_remove(S, e)            bt_remove(S, e)
#define set_copy(S)                 bt_copy(S)
#define set_has(S, e)               bt_has(S, e);
#define set_traverse(S, f, p)       bt_traverse_keys(S, f, p)
#define set_traverse_r(S, f, p)     bt_traverse_keys_r(S, f, p)

set *set_union(set *S1, set *S2);
set *set_intersection(set *S1, set *S2);
set *set_difference(set *S1, set *S2);

#endif // _set_h
