#include <stdlib.h>

#include "set.h"
#include "stack.h"

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

/***************************************************************************************
 *
 * Set Set_copy(Set S);
 *
 * Return an exact copy of the given set. Adapter for _rbt_copy.
 *
 **************************************************************************************/

Set Set_copy(Set S)
{
    Set C = Set_new(S->element_type);
    _rbt_copy(C, S);
    return C;
}

/***************************************************************************************
 *
 * static inline int _copy_data_into_other_rbt(_rbt_node *n, void *T)
 *
 * Callback for _rbt_traverse: Insert the data members in a red-black tree into the
 * given other tree.
 *
 **************************************************************************************/

static inline int _copy_data_into_other_rbt(_rbt_node *n, void *T)
{
    int rc = _rbt_insert(T, n->data);
    return rc >= 0 ? 0 : -1;
}

/***************************************************************************************
 *
 * Set Set_union(Set S1, Set S2);
 *
 * Return a new set that contains all elements that are in either of S1 and S2 or both,
 * or NULL on error.
 *
 * Create an exact copy of the larger set in linear time and traverse the smaller set,
 * inserting each of its elements into the copy. If m, n are the sizes of the larger and
 * the smaller set, respectively, this requires approximately m + n * log (m + n) steps.
 *
 **************************************************************************************/

Set Set_union(Set S1, Set S2)
{
    Set larger = Set_size(S1) > Set_size(S2) ? S1 : S2;
    Set smaller = larger == S1 ? S2 : S1;

    Set U = Set_copy(larger);
    _rbt_traverse(smaller, _copy_data_into_other_rbt, U);
    return U;
}

/***************************************************************************************
 *
 * static inline int _push_data_pointer_to_stack(_rbt_node *n, void *stack);
 *
 * Callback for _rbt_traverse: Push pointers to the data elements in a red-black tree
 * onto the given stack.
 *
 **************************************************************************************/

static inline int _push_data_pointer_to_stack(_rbt_node *n, void *stack)
{
    return Stack_push(stack, &n->data);
}

/***************************************************************************************
 *
 * Set Set_intersection(Set S1, Set S2);
 *
 * Return a new set that contains all elements that are in both S1 and S2
 * or NULL on error.
 *
 * The naive approach would be to create a new set, then traverse both sets and insert
 * every element into the new set. If m is the size of the larger set, and n of the
 * other one, that would require m + n insertions with a logarithmic number of steps
 * each, resulting in a total of approximately (m + n) * log n steps.
 *
 * We trade space for time and create two stacks of pointers to data elements as an
 * in-between data structure in linear time. Then we walk through both stacks
 * simultaneously, comparing elements and inserting only elements that are in both
 * stacks. This gives us slightly fewer m + n + n * log n steps.
 *
 **************************************************************************************/

Set Set_intersection(Set S1, Set S2)
{
    check_ptr(S1);
    check_ptr(S2);
    check(S1->element_type == S2->element_type, "Element types don't match.");

    int rc = -1;

    Set I = Set_new(S1->element_type);
    check(I != NULL, "Failed to create new set.");

    _stack stack1, stack2;
    rc = _stack_init(&stack1, &pointer_type);
    check(rc == 0, "Failed to initialize first stack.");

    rc = _stack_init(&stack2, &pointer_type);
    check(rc == 0, "Failed to initialize second stack.");

    rc = _rbt_traverse_r(S1, _push_data_pointer_to_stack, &stack1);
    check(rc == 0, "Failed to copy data pointers from first set onto first stack.");

    rc = _rbt_traverse_r(S2, _push_data_pointer_to_stack, &stack2);
    check(rc == 0, "Failed to copy data pointers from second set onto second stack.");

    void *v1, *v2;
    rc = Stack_pop(&stack1, &v1);
    check(rc == 0, "Failed to pop value from first stack.");
    rc = Stack_pop(&stack2, &v2);
    check(rc == 0, "Failed to pop value from second stack.");

    int comp;
    for ( ;; ) {
        comp = TypeInterface_compare(S1->element_type, v1, v2);
        if (comp < 0) {
            if (Stack_size(&stack1) == 0) break;
            rc = Stack_pop(&stack1, &v1);
            check(rc == 0, "Failed to pop value from first stack.");
        } else if (comp > 0) {
            if (Stack_size(&stack1) == 0) break;
            rc = Stack_pop(&stack2, &v2);
            check(rc == 0, "Failed to pop value from second stack.");
        } else { /* comp == 0 */
            rc = Set_insert(I, v1);
            check(rc == 0, "Failed to insert value into new set.");

            if (Stack_size(&stack1) == 0 || Stack_size(&stack2) == 0) break;
            rc = Stack_pop(&stack1, &v1);
            check(rc == 0, "Failed to pop value from first stack.");
            rc = Stack_pop(&stack2, &v2);
            check(rc == 0, "Failed to pop value from second stack.");
        }
    }

    Stack_clear(&stack1);
    Stack_clear(&stack2);
    return I;

error:
    return NULL;
}
