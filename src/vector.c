#include <stdlib.h>
#include <string.h>

#include "container_tools.h"
#include "debug.h"
#include "vector.h"

int _vector_init(_vector *V,
                 const size_t element_size,
                 copy_f copy_element,
                 destroy_f destroy_element)
{
    V->data = malloc(VECTOR_MIN_CAPACITY * element_size);
    check_alloc(V->data);

    V->size = 0;
    V->capacity = VECTOR_MIN_CAPACITY;
    V->element_size = element_size;
    V->destroy_element = destroy_element;
    V->copy_element = copy_element;

    return 0;
error:
    return -1;
}

Vector Vector_new(const size_t element_size,
                  copy_f copy_element,
                  destroy_f destroy_element)
{
    _vector *V = malloc(sizeof(*V));
    check_alloc(V);

    check(!_vector_init(V, element_size, copy_element, destroy_element),
            "Failed to initialize new vector.");

    return V;
error:
    if (V->data) free(V->data);
    if (V) free(V);
    return NULL;
}

void _vector_dealloc(_vector *V)
{
    if (V && V->data) {
        Vector_clear(V);
        free(V->data);
    }
}

void Vector_delete(Vector V)
{
    if (V) {
        _vector_dealloc(V);
        free(V);
    }
}

/* Reserve internal memory for at least `capacity` elements. */
int Vector_reserve(Vector V, const size_t capacity)
{
    check_ptr(V);

    /* We expand by doubling the space: Compute the smallest power of 2 > capacity. */
    size_t c = VECTOR_MIN_CAPACITY;
    while (c < capacity) c <<= 1;

    if (c == V->capacity) return 0;

    /* If we have an element destructor, we need to make sure that all elements
     * that we are going to cut off are properly destroyed. */
    if (c < V->size && V->destroy_element) {
        for (size_t i = c; i < V->size; ++i) {
            V->destroy_element(*(void**)(V->data + i * V->element_size));
        }
    }

    /* Assume that reallocarray doesn't touch the original array if it fails.
     * Also assume that nested types remain intact when only the top level data
     * is moved. */
    char *new_data = reallocarray(V->data, c, V->element_size);
    check(new_data != NULL, "Failed to reserve the requested amonut of memory.");
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

/* Contract the internal storage if and as possible. Does not necessarily
 * shrink to the exact size. */
int Vector_shrink_to_fit(Vector V)
{
    check_ptr(V);

    if (V->size > V->capacity >> 1) return 0;

    /* Shrink to at least twice the currently used memory to avoid expanding
     * again immediately. */
    size_t c = V->capacity;
    while (c > V->size << 1) c >>= 1;

    /* Assumptions as above. */
    char *new_data = reallocarray(V->data, c, V->element_size);
    check(new_data != NULL, "Failed to reduce the internal memory.");
    V->data = new_data;
    V->capacity = c;

    return 0;
error:
    return -1;
}

void Vector_clear(Vector V)
{
    if (V && V->data) {
        if (V->destroy_element) {
            for (size_t i = 0; i < V->size; ++i) {
                V->destroy_element(*(void**)(V->data + i * V->element_size));
            }
        }
        V->size = 0;
        Vector_reserve(V, VECTOR_MIN_CAPACITY);
    }
}

int Vector_get(const Vector V, const size_t i, void *out)
{
    check_ptr(V);
    check_ptr(out);
    check(i < V->size, "Index out of range: %lu > %lu", i, V->size);

    if (V->copy_element) {
        V->copy_element(out, V->data + i * V->element_size);
    } else {
        memmove(out, V->data + i * V->element_size, V->element_size);
    }
    return 0;
error:
    return -1;
}

int Vector_set(Vector V, const size_t i, const void *in)
{
    check_ptr(V);
    check_ptr(in);
    check(i <= V->size, "Index out of range: %lu > %lu", i, V->size);

    if (i == V->size) {
        if (V->size >= V->capacity) {
            check(!Vector_reserve(V, V->capacity << 1), "Failed to expand.");
        }
        ++V->size;
    } else if (i < V->size && V->destroy_element) {
        V->destroy_element(*(void**)(V->data + i * V->element_size));
    }

    if (V->copy_element) {
        V->copy_element(V->data + i * V->element_size, in);
    } else {
        memmove(V->data + i * V->element_size, in, V->element_size);
    }

    return 0;
error:
    return -1;
}

int Vector_insert(Vector V, const size_t i, const void *in)
{
    check_ptr(V);
    check_ptr(in);
    check(i <= V->size, "Index out of range: %lu > %lu", i, V->size);

    if (V->size >= V->capacity) {
        check(!Vector_reserve(V, V->capacity << 1), "Failed to expand.");
    }

    if (i < V->size) {
        /* Assume that nested types remain intact when only the top level data
         * is moved. */
        memmove(V->data + (i + 1) * V->element_size,
                V->data + i * V->element_size,
                (V->size - i) * V->element_size);
    }

    if (V->copy_element) {
        V->copy_element(V->data + i * V->element_size, in);
    } else {
        memmove(V->data + i * V->element_size, in, V->element_size);
    }

    ++V->size;

    return 0;
error:
    return -1;
}

/* Delete the element at index i. All subsequent elements are moved, so any
 * pointers into the Vector become invalid. */
int Vector_remove(Vector V, const size_t i)
{
    check_ptr(V);
    check(i < V->size, "Index out of range: %lu > %lu", i, V->size);

    if (V->destroy_element) {
        V->destroy_element(*(void**)(V->data + i * V->element_size));
    }

    if (i < V->size - 1) {
        /* Assume that nested types remain intact when only the top level data
         * is moved. */
        memmove(V->data + i * V->element_size,
                V->data + (i + 1) * V->element_size,
                (V->size - (i + 1)) * V->element_size);
    }

    --V->size;

    /* Contract if we are below 25% memory usage. If this fails it's not a
     * disaster so we just log it. */
    if (V->size < (V->capacity >> 2) && V->capacity > VECTOR_MIN_CAPACITY) {
        int rc = Vector_shrink_to_fit(V);
        if (rc != 0) debug("Failed to contract internal memory.");
    }

    return 0;
error:
    return -1;
}

int Vector_push_back(Vector V, const void *in)
{
    return Vector_set(V, V->size, in);
}

int Vector_pop_back(Vector V, void *out)
{
    check_ptr(V);
    check(V->size > 0, "Attempt to pop_back from empty vector.");

    if (out) {
        check(!Vector_get(V, V->size - 1, out), "Failed to get element.");
    }

    check(!Vector_remove(V, V->size - 1), "Failed to remove element.");

    return 0;
error:
    return -1;
}
