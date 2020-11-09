#include <stdlib.h>

#include "set.h"

Set Set_new(TypeInterface *element_type)
{
    check_ptr(element_type);

    _rbt *S = malloc(sizeof(*S));
    check_alloc(S);

    S->element_type = element_type;
    S->size = 0;
    S->root = NULL;

    return S;
error:
    return NULL;
}

void Set_delete(Set S)
{
    if (S) {
        _rbt_clear(S);
        free(S);
    }
}

int _rbt_copy_node_to(_rbt_node *n, void *p)
{
    int rc = _rbt_insert(p, n->data);
    return rc >= 0 ? 0 : -1;
}

Set Set_copy(Set S)
{
    Set C = Set_new(S->element_type);
    _rbt_traverse(S, _rbt_copy_node_to, C);
    return C;
}

Set Set_union(Set S1, Set S2)
{
    Set U = Set_copy(S1);
    _rbt_traverse(S2, _rbt_copy_node_to, U);
    return U;
}
