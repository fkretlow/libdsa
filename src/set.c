/*************************************************************************************************
 *
 * set.c
 *
 * Implementation of the set methods that are not covered by generic BST methods.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <stdlib.h>

#include "check.h"
#include "log.h"
#include "set.h"
#include "vector.h"

set *set_new(t_intf *dt)
{
    check_ptr(dt);

    set *S = malloc(sizeof(*S));
    check_alloc(S);

    set_initialize(S, dt);

    return S;
error:
    return NULL;
}

/* set *set_union(set *S1, set *S2)
 * Return a new set that contains all elements that are in either of S1 and S2 or both, or NULL on
 * error.
 * Create an exact copy of the larger set in linear time and traverse the smaller set, inserting
 * each of its elements into the copy. If m, n are the sizes of the larger and the smaller set
 * respectively, this gives an upper bound of m + n * log (m + n) steps. */
static inline int insert_into_set(void *e, void *S)
{
    int rc = set_insert(S, e);
    return rc >= 0 ? 0 : -1;
}

set *set_union(set *S1, set *S2)
{
    set *larger = set_count(S1) > set_count(S2) ? S1 : S2;
    set *smaller = larger == S1 ? S2 : S1;

    set *U = set_copy(larger);
    set_traverse(smaller, insert_into_set, U);
    return U;
}

/* set *set_intersection(set *S1, set *S2)
 * Return a new set that contains all elements that are in both S1 and S2 or NULL on error.
 * The naive approach would be to create a new set, then traverse both sets and insert every
 * element into the new set. If m is the size of the larger set, and n of the other one, that
 * would require m + n insertions with a logarithmic number of steps each, resulting in an upper
 * bound of (m + n) * log n steps.  We trade space for time and create two arrays of pointers to
 * data elements as an in-between data structure in linear time. Then we walk through both arrays
 * simultaneously, comparing elements and inserting only elements that are in both arrays. This
 * should give us slightly fewer m + n + n * log n steps. */

static inline int push_pointer_into_vector(void *e, void *V)
{
    int rc = vector_push_back(V, &e);
    return rc >= 0 ? 0 : -1;
}

set *set_intersection(set *S1, set *S2)
{
    set *I = NULL;
    vector V1, V2;
    void *e1, *e2;
    int comp;

    check_ptr(S1);
    check_ptr(S2);
    check(S1->key_type == S2->key_type, "Element types don't match.");

    I = set_new(S1->key_type);
    vector_initialize(&V1, &pointer_type);
    vector_initialize(&V2, &pointer_type);

    vector_reserve(&V1, set_count(S1));
    vector_reserve(&V2, set_count(S2));

    set_traverse_r(S1, push_pointer_into_vector, &V1);
    set_traverse_r(S2, push_pointer_into_vector, &V2);

    vector_pop_back(&V1, &e1);
    vector_pop_back(&V2, &e2);

    for ( ;; ) {
        comp = t_compare(S1->key_type, e1, e2);
        if (comp < 0) {
            if (vector_count(&V1) == 0) break;
            vector_pop_back(&V1, &e1);
        } else if (comp > 0) {
            if (vector_count(&V2) == 0) break;
            vector_pop_back(&V2, &e2);
        } else { /* comp == 0 */
            set_insert(I, e1);
            if (vector_count(&V1) == 0 || vector_count(&V2) == 0) break;
            vector_pop_back(&V1, &e1);
            vector_pop_back(&V2, &e2);
        }
    }

    vector_destroy(&V1);
    vector_destroy(&V2);

    return I;

error:
    if (I) set_delete(I);
    return NULL;
}

/* set *set_difference(set *S1, set *S2)
 * Return a new set that contains all elements that are in the first but not in the second set.
 * Create an exact copy of the larger set and traverse the smaller set, removing each of its
 * elements from the copy. */
static int remove_from_set(void *e, void *S)
{
    int rc = set_remove(S, e);
    return rc >= 0 ? 0 : 1;
}

set *set_difference(set *S1, set *S2)
{
    set *D = set_copy(S1);
    set_traverse(S2, remove_from_set, D);
    return D;
}
