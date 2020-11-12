#ifndef _set_h
#define _set_h

#include "red_black_tree.h"
#include "type_interface.h"

typedef RBTree Set;

#define Set_size(S) (S)->size

#define Set_initialize(S, key_type) RBTree_initialize(S, key_type)
Set *Set_new(TypeInterface *key_type);
void Set_delete(Set *S);
#define Set_clear(S) RBTree_clear(S)

#define Set_insert(S, v) RBTree_insert(S, v);
#define Set_remove(S, v) RBTree_remove(S, v);
#define Set_has(S, v) RBTree_has(S, v);
#define Set_traverse(S, f, p) RBTree_traverse(S, f, p)
#define Set_traverse_r(S, f, p) RBTree_traverse_r(S, f, p)

Set *Set_copy(Set *S);
Set *Set_union(Set *S1, Set *S2);
Set *Set_intersection(Set *S1, Set *S2);
Set *Set_difference(Set *S1, Set *S2);

#endif // _set_h
