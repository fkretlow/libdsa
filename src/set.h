#ifndef _set_h
#define _set_h

#include "rbt.h"
#include "type_interface.h"

typedef _rbt *Set;

#define Set_size(S) (S)->size

Set Set_new(TypeInterface *element_type);
void Set_delete(Set S);
#define Set_clear(S) _rbt_clear(S)

#define Set_insert(S, v) _rbt_insert(S, v);
#define Set_remove(S, v) _rbt_remove(S, v);
#define Set_has(S, v) _rbt_has(S, v);

Set Set_copy(Set S);
Set Set_union(Set S1, Set S2);
Set Set_intersection(Set S1, Set S2);
Set Set_difference(Set S1, Set S2);

#endif // _set_h
