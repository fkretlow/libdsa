#include <stdlib.h>

#include "set.h"
#include "stack.h"
#include "vector.h"

Set *Set_new(TypeInterface *key_type)
{
    check_ptr(key_type);

    Set *S = malloc(sizeof(*S));
    check_alloc(S);

    Set_initialize(S, key_type);

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

static inline int _copy_data_into_otherRBTree(RBTreeNode *n, void *T)
{
    int rc = RBTree_insert(T, n->key);
    return rc >= 0 ? 0 : -1;
}

Set *Set_union(Set *S1, Set *S2)
{
    Set *larger = Set_size(S1) > Set_size(S2) ? S1 : S2;
    Set *smaller = larger == S1 ? S2 : S1;

    Set *U = Set_copy(larger);
    Set_traverse(smaller, _copy_data_into_otherRBTree, U);
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

static inline int _push_data_pointer_to_vector(RBTreeNode *n, void *v)
{
    return Vector_push_back(v, &n->key);
}

Set *Set_intersection(Set *S1, Set *S2)
{
    Set *I = NULL;
    Vector V1, V2;
    void *e1, *e2;
    int comp;

    check_ptr(S1);
    check_ptr(S2);
    check(S1->key_type == S2->key_type, "Element types don't match.");

    I = Set_new(S1->key_type);
    Vector_initialize(&V1, &pointer_type);
    Vector_initialize(&V2, &pointer_type);

    Vector_reserve(&V1, Set_size(S1));
    Vector_reserve(&V2, Set_size(S2));

    Set_traverse_r(S1, _push_data_pointer_to_vector, &V1);
    Set_traverse_r(S2, _push_data_pointer_to_vector, &V2);

    Vector_pop_back(&V1, &e1);
    Vector_pop_back(&V2, &e2);

    for ( ;; ) {
        comp = TypeInterface_compare(S1->key_type, e1, e2);
        if (comp < 0) {
            if (Vector_size(&V1) == 0) break;
            Vector_pop_back(&V1, &e1);
        } else if (comp > 0) {
            if (Vector_size(&V2) == 0) break;
            Vector_pop_back(&V2, &e2);
        } else { /* comp == 0 */
            Set_insert(I, e1);
            if (Vector_size(&V1) == 0 || Vector_size(&V2) == 0) break;
            Vector_pop_back(&V1, &e1);
            Vector_pop_back(&V2, &e2);
        }
    }

    Vector_destroy(&V1);
    Vector_destroy(&V2);
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

static int _remove_data_from_otherRBTree(RBTreeNode *n, void *T)
{
    return RBTree_remove(T, n->key) >= 0 ? 0 : -1;
}

Set *Set_difference(Set *S1, Set *S2)
{
    Set *D = Set_copy(S1);
    Set_traverse(S2, _remove_data_from_otherRBTree, D);
    return D;
}
