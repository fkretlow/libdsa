/*************************************************************************************************
 *
 * vector.c
 *
 * Implementation of the vector interface defined in vector.h.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "vector.h"

/* int vector_initialize(vector *V, t_intf *dt)
 * Initialize the vector at V with the type interface dt. We assume there's enough space. Returns
 * 0 on success, or -1 on error. */
int vector_initialize(vector *V, t_intf *dt)
{
    check_ptr(V);
    check_ptr(dt);
    check(dt->size, "no data size");

    V->data = malloc(VECTOR_MIN_CAPACITY * t_size(dt));
    check_alloc(V->data);

    V->count = 0;
    V->capacity = VECTOR_MIN_CAPACITY;
    V->data_type = dt;

    return 0;
error:
    return -1;
}

/* vector *vector_new(t_intf *dt)
 * Create a new vector on the heap and initialize it with the type interface dt. Return a pointer
 * to the new vector or NULL on error. */
vector *vector_new(t_intf *dt)
{
    vector *V = malloc(sizeof(*V));
    check_alloc(V);

    int rc = vector_initialize(V, dt);
    check_rc(rc, "vector_initialize");

    return V;
error:
    if (V->data) free(V->data);
    if (V) free(V);
    return NULL;
}

/* void vector_destroy(vector *V)
 * void vector_delete (vector *V)
 * Destroy V including all its content, free the allocated storage and reset the struct.
 * vector_delete also calls `free` on V. */
void vector_destroy(vector *V)
{
    if (V && V->data) {
        vector_clear(V);
        free(V->data);
        V->data = NULL;
        V->count = 0;
        V->capacity = 0;
    }
}

void vector_delete(vector *V)
{
    if (V) {
        vector_destroy(V);
        free(V);
    }
}

/* int vector_reserve(vector *V, const size_t n)
 * Allocate internal memory for at least n elements, to be precise, for 2^m elements, where 2^m is
 * the smallest power of two greater than n. Return 0 on success, or -1 on error. */
int vector_reserve(vector *V, const size_t n)
{
    check_ptr(V);

    /* We expand by doubling the space: Compute the smallest power of 2 > n. */
    size_t c = VECTOR_MIN_CAPACITY;
    while (c < n) c <<= 1;

    if (c == V->capacity) return 0;

    size_t s = t_size(V->data_type);

    /* If we have an element destructor, we need to make sure that all elements that we are going
     * to cut off are properly destroyed. */
    if (c < V->count) {
        for (size_t i = c; i < V->count; ++i) {
            t_destroy(V->data_type, V->data + i * s);
        }
        V->count = c;
    }

    /* We can't assume that all types of objects remain intact when only the top level data is
     * moved, so we can't use realloc. Allocate the new storage, move all elements there, destroy
     * the old storage. */
    char *new_data = malloc(c * s);
    check_alloc(new_data);
    for (size_t i = 0; i < V->count; ++i) {
        t_move(V->data_type, new_data + i * s, V->data + i * s);
    }

    free(V->data);
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

/* int vector_shrink_to_fit(vector *V)
 * Contract the internal storage if and as possible. The new capacity will be a power of two that
 * is at least twice the current count. Returns 0 on success, or -1 on error. */
int vector_shrink_to_fit(vector *V)
{
    check_ptr(V);

    if (V->count > V->capacity >> 1 || V->capacity <= VECTOR_MIN_CAPACITY) return 0;

    /* Contract to a power of two, at least twice the currently used memory to avoid expanding
     * again immediately. */
    size_t c = V->capacity;
    while (c > V->count << 1) c >>= 1;

    size_t s = t_size(V->data_type);

    char *new_data = malloc(c * s);
    check_alloc(new_data);

    for (size_t i = 0; i < V->count; ++i) {
        t_move(V->data_type, new_data + i * s, V->data + i * s);
    }

    free(V->data);
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

/* void vector_clear(vector *V)
 * Delete all elements of V and contract to the initial size. */
void vector_clear(vector *V)
{
    if (V && V->data) {
        for (size_t i = 0; i < V->count; ++i) {
            t_destroy(V->data_type, V->data + i * t_size(V->data_type));
        }
        V->count = 0;
        vector_reserve(V, VECTOR_MIN_CAPACITY);
    }
}

/* int vector_set(vector *V, const size_t i, const void *e)
 * Copy the object e to the slot at index i in V, destroying the previous element. i cannot be
 * larger than V->count (can't leave holes in V). Returns 1 if an element was added at the end, 0
 * if one was overwritten, and -1 on error. */
int vector_set(vector *V, const size_t i, const void *e)
{
    check_ptr(V);
    check_ptr(e);
    check(i <= V->count, "index out of range: %lu > %lu", i, V->count);

    int rc;
    size_t s = t_size(V->data_type);

    if (i == V->count) {
        if (V->count >= V->capacity) {
            rc = vector_reserve(V, V->capacity << 1);
            check_rc(rc, "vector_reserve");
        }
        ++V->count;
        rc = 1;

    } else if (i < V->count) {
        t_destroy(V->data_type, V->data + i * s);
        rc = 0;
    }

    t_copy(V->data_type, V->data + i * s, e);

    return rc;
error:
    return -1;
}

/* int vector_insert(vector *V, const size_t i, const void *e)
 * Insert e into V at index i, moving all subsequent elements including the one at index i one
 * slot to the right (pointers into the array become invalid). Returns 1 if an element was added,
 * or -1 on error. */
int vector_insert(vector *V, const size_t i, const void *e)
{
    check_ptr(V);
    check_ptr(e);
    check(i < V->count, "index out of range");

    int rc;

    if (V->count >= V->capacity) {
        rc = vector_reserve(V, V->capacity << 1);
        check_rc(rc, "vector_reserve");
    }

    /* Move all subsequent elements one slot to the right, beginning with the last one so as to
     * not overwrite anything. */
    size_t s = t_size(V->data_type);
    if (V->count) {
        for (size_t j = V->count - 1; j >= i && j != SIZE_MAX; --j) {
            t_move(V->data_type, V->data + (j + 1) * s, V->data + j * s);
        }
    }

    t_copy(V->data_type, V->data + i * s, e);
    ++V->count;

    return 1;
error:
    return -1;
}

/* int vector_remove(vector *V, const size_t i)
 * Delete the element at index i. All subsequent elements are moved, so any pointers into the
 * vector become invalid. Returns 1 if an element was removed, or -1 on error. */
int vector_remove(vector *V, const size_t i)
{
    check_ptr(V);
    check(i < V->count, "index out of range");

    size_t s = t_size(V->data_type);
    t_destroy(V->data_type, V->data + i * s);

    /* Move all subsequent elements one slot to the left. */
    if (i < V->count - 1) {
        for (size_t j = i + 1; j < V->count; ++j) {
            t_move(V->data_type, V->data + (j - 1) * s, V->data + j * s);
        }
    }

    --V->count;

    /* Contract if we are below 25% memory usage. If this fails it's not a disaster so we just cry
     * in the shower. */
    if (V->count < (V->capacity >> 2) && V->capacity > VECTOR_MIN_CAPACITY) {
        int rc = vector_shrink_to_fit(V);
        if (rc < 0) log_warn("failed to contract internal memory");
    }

    return 1;
error:
    return -1;
}

/* int vector_push_back(vector *V, const void *e)
 * Add e to V at the end. Shorthand for vector_set(V, vector_count(V), e). Returns 1 if an element
 * was added, or -1 on error. */
int vector_push_back(vector *V, const void *e)
{
    return vector_set(V, V->count, e);
}

/* int vector_pop_back(vector *V, void *out)
 * Pop the last element, moving it where out points unless out is NULL. We assume there's enough
 * space. Return 1 if an element was deleted, 0 if the vector was empty, or -1 on error. */
int vector_pop_back(vector *V, void *out)
{
    check_ptr(V);
    if (V->count == 0) return 0;

    if (out) t_move(V->data_type, out, vector_last(V));
    else t_destroy(V->data_type, vector_last(V));
    --V->count;

    if (V->count < (V->capacity >> 2) && V->capacity > VECTOR_MIN_CAPACITY) {
        int rc = vector_shrink_to_fit(V);
        if (rc < 0) log_warn("failed to contract internal memory");
    }

    return 1;
error:
    return -1;
}
