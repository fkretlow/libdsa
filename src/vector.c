#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "vector.h"

int vector_initialize(vector *V, t_intf *dt)
{
    check_ptr(V);
    check_ptr(dt);

    V->data = malloc(VECTOR_MIN_CAPACITY * t_size(dt));
    check_alloc(V->data);

    V->count = 0;
    V->capacity = VECTOR_MIN_CAPACITY;
    V->data_type = dt;

    return 0;
error:
    return -1;
}

vector *vector_new(t_intf *dt)
{
    vector *V = malloc(sizeof(*V));
    check_alloc(V);

    int rc = vector_initialize(V, dt);
    check(rc == 0, "failed to initialize new vector");

    return V;
error:
    if (V->data) free(V->data);
    if (V) free(V);
    return NULL;
}

void vector_destroy(vector *V)
{
    if (V && V->data) {
        vector_clear(V);
        free(V->data);
        V->data = NULL;
        V->count = 0;
    }
}

void vector_delete(vector *V)
{
    if (V) {
        vector_destroy(V);
        free(V);
    }
}

/* Reserve internal memory for at least `capacity` elements. */
int vector_reserve(vector *V, const size_t capacity)
{
    check_ptr(V);

    /* We expand by doubling the space: Compute the smallest power of 2 > capacity. */
    size_t c = VECTOR_MIN_CAPACITY;
    while (c < capacity) c <<= 1;

    if (c == V->capacity) return 0;

    /* If we have an element destructor, we need to make sure that all elements
     * that we are going to cut off are properly destroyed. */
    if (c < V->count) {
        for (size_t i = c; i < V->count; ++i) {
            t_destroy(V->data_type, V->data + i * t_size(V->data_type));
        }
    }

    /* Assume that reallocarray doesn't touch the original array if it fails.
     * Also assume that nested types remain intact when only the top level data
     * is moved. */
    char *new_data = reallocarray(V->data, c, t_size(V->data_type));
    check(new_data != NULL, "Failed to reserve the requested amonut of memory.");
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

/* Contract the internal storage if and as possible. Does not necessarily
 * shrink to the exact size. */
int vector_shrink_to_fit(vector *V)
{
    check_ptr(V);

    if (V->count > V->capacity >> 1 || V->capacity <= VECTOR_MIN_CAPACITY) return 0;

    /* Shrink to at least twice the currently used memory to avoid expanding
     * again immediately. */
    size_t c = V->capacity;
    while (c > V->count << 1) c >>= 1;

    /* Assumptions as above. TODO: Use t_move. */
    char *new_data = reallocarray(V->data, c, t_size(V->data_type));
    check(new_data != NULL, "failed to shrink the internal memory with reallocarray");
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

void vector_clear(vector *V)
{
    if (V && V->data) {
        for (size_t i = 0; i < V->count; ++i) {
            t_destroy(V->data_type,
                                  V->data + i * t_size(V->data_type));
        }
        V->count = 0;
        vector_reserve(V, VECTOR_MIN_CAPACITY);
    }
}

int vector_set(vector *V, const size_t i, const void *in)
{
    check_ptr(V);
    check_ptr(in);
    check(i <= V->count, "Index out of range: %lu > %lu", i, V->count);

    if (i == V->count) {
        if (V->count >= V->capacity) {
            check(!vector_reserve(V, V->capacity << 1), "Failed to expand.");
        }
        ++V->count;
    } else if (i < V->count) {
        t_destroy(V->data_type,
                              V->data + i * t_size(V->data_type));
    }

    t_copy(V->data_type,
                       V->data + i * t_size(V->data_type),
                       in);

    return 0;
error:
    return -1;
}

int vector_insert(vector *V, const size_t i, const void *in)
{
    check_ptr(V);
    check_ptr(in);
    check(i <= V->count, "Index out of range: %lu > %lu", i, V->count);

    if (V->count >= V->capacity) {
        check(!vector_reserve(V, V->capacity << 1), "Failed to expand.");
    }

    if (i < V->count) {
        /* Assume that nested types remain intact when only the top level data
         * is moved. */
        memmove(V->data + (i + 1) * t_size(V->data_type),
                V->data + i * t_size(V->data_type),
                (V->count - i) * t_size(V->data_type));
    }

    t_copy(V->data_type,
                       V->data + i * t_size(V->data_type),
                       in);
    ++V->count;

    return 0;
error:
    return -1;
}

/* Delete the element at index i. All subsequent elements are moved, so any
 * pointers into the vector become invalid. */
int vector_remove(vector *V, const size_t i)
{
    check_ptr(V);
    check(i < V->count, "Index out of range: %lu > %lu", i, V->count);

    t_destroy(V->data_type, V->data + i * t_size(V->data_type));

    if (i < V->count - 1) {
        /* Assume that nested types remain intact when only the top level data
         * is moved. */
        memmove(V->data + i * t_size(V->data_type),
                V->data + (i + 1) * t_size(V->data_type),
                (V->count - (i + 1)) * t_size(V->data_type));
    }

    --V->count;

    /* Contract if we are below 25% memory usage. If this fails it's not a
     * disaster so we just log it. */
    if (V->count < (V->capacity >> 2) && V->capacity > VECTOR_MIN_CAPACITY) {
        int rc = vector_shrink_to_fit(V);
        if (rc < 0) log_warn("failed to contract internal memory");
    }

    return 0;
error:
    return -1;
}

int vector_push_back(vector *V, const void *in)
{
    return vector_set(V, V->count, in);
}

/* int vector_pop_back(vector *V, void *out)
 * Pop the last element, moving it to out unless out is NULL. Return 1 if there was one, 0 if the
 * vector was empty, or -1 on error. */

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
