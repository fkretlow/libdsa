#include <stdlib.h>

#include "set.h"
#include "stack.h"
#include "vector.h"

Set *Set_new(t_intf *element_type)
{
    check_ptr(element_type);

    Set *S = malloc(sizeof(*S));
    check_alloc(S);

    Set_initialize(S, element_type);

    return S;
error:
    return NULL;
}

void Set_delete(Set *S)
{
    if (S) {
        RBTree_clear(S);
        free(S);
    }
}

/***************************************************************************************
 *
 * Set *Set_copy(Set *S);
 *
 * Return an exact copy of the given set. Adapter for RBTree_copy.
 *
 **************************************************************************************/

Set *Set_copy(Set *S)
{
    Set *C = Set_new(S->key_type);
    RBTree_copy(C, S);
    return C;
}

/***************************************************************************************
 *
 * Set *Set_union(Set *S1, Set *S2);
 *
 * Return a new set that contains all elements that are in either of S1 and S2 or both,
 * or NULL on error.
 *
 * Create an exact copy of the larger set in linear time and traverse the smaller set,
 * inserting each of its elements into the copy. If m, n are the sizes of the larger and
 * the smaller set respectively, this gives an upper bound of m + n * log (m + n) steps.
 *
 **************************************************************************************/

static inline int insert_into_set(void *e, void *S)
{
    int rc = Set_insert(S, e);
    return rc >= 0 ? 0 : -1;
}

Set *Set_union(Set *S1, Set *S2)
{
    Set *larger = Set_size(S1) > Set_size(S2) ? S1 : S2;
    Set *smaller = larger == S1 ? S2 : S1;

    Set *U = Set_copy(larger);
    Set_traverse(smaller, insert_into_set, U);
    return U;
}

/***************************************************************************************
 *
 * Set *Set_intersection(Set *S1, Set *S2);
 *
 * Return a new set that contains all elements that are in both S1 and S2
 * or NULL on error.
 *
 * The naive approach would be to create a new set, then traverse both sets and insert
 * every element into the new set. If m is the size of the larger set, and n of the
 * other one, that would require m + n insertions with a logarithmic number of steps
 * each, resulting in an upper bound of (m + n) * log n steps.
 *
 * We trade space for time and create two arrays of pointers to data elements as an
 * in-between data structure in linear time. Then we walk through both arrays
 * simultaneously, comparing elements and inserting only elements that are in both
 * arrays. This should give us slightly fewer m + n + n * log n steps.
 *
 **************************************************************************************/

static inline int push_pointer_into_vector(void *e, void *V)
{
    return vector_push_back(V, &e);
}

Set *Set_intersection(Set *S1, Set *S2)
{
    Set *I = NULL;
    vector V1, V2;
    void *e1, *e2;
    int comp;

    check_ptr(S1);
    check_ptr(S2);
    check(S1->key_type == S2->key_type, "Element types don't match.");

    I = Set_new(S1->key_type);
    vector_initialize(&V1, &pointer_type);
    vector_initialize(&V2, &pointer_type);

    vector_reserve(&V1, Set_size(S1));
    vector_reserve(&V2, Set_size(S2));

    Set_traverse_r(S1, push_pointer_into_vector, &V1);
    Set_traverse_r(S2, push_pointer_into_vector, &V2);

    vector_pop_back(&V1, &e1);
    vector_pop_back(&V2, &e2);

    for ( ;; ) {
        comp = t_compare(S1->key_type, e1, e2);
        if (comp < 0) {
            if (vector_size(&V1) == 0) break;
            vector_pop_back(&V1, &e1);
        } else if (comp > 0) {
            if (vector_size(&V2) == 0) break;
            vector_pop_back(&V2, &e2);
        } else { /* comp == 0 */
            Set_insert(I, e1);
            if (vector_size(&V1) == 0 || vector_size(&V2) == 0) break;
            vector_pop_back(&V1, &e1);
            vector_pop_back(&V2, &e2);
        }
    }

    vector_destroy(&V1);
    vector_destroy(&V2);

    return I;

error:
    if (I) Set_delete(I);
    return NULL;
}

/***************************************************************************************
 *
 * Set *Set_difference(Set *S1, Set *S2);
 *
 * Return a new set that contains all elements that are in the first but not in the
 * second set.
 *
 * Create an exact copy of the larger set and traverse the smaller set, removing each of
 * its elements from the copy.
 *
 **************************************************************************************/

static int remove_from_set(void *e, void *S)
{
    int rc = Set_remove(S, e);
    return rc >= 0 ? 0 : 1;
}

Set *Set_difference(Set *S1, Set *S2)
{
    Set *D = Set_copy(S1);
    Set_traverse(S2, remove_from_set, D);
    return D;
}
