#include <stdlib.h>
#include <string.h>

#include "container_tools.h"
#include "debug.h"
#include "vector.h"

static int __Vector_resize(Vector* v, const size_t capacity)
{
    size_t c = VECTOR_MIN_CAPACITY;
    while (c < capacity) c <<= 1;

    if (c == v->max) return 0;

    char* new_data = malloc(c * v->element_size);
    check_alloc(new_data);

    memmove(new_data, v->data, (c < v->end ? c : v->end) * v->element_size);

    if (c < v->end && v->destroy) {
        for (size_t i = c; i < v->end; ++i) {
            v->destroy(v->data + i * v->element_size);
        }
    }

    free(v->data);
    v->data = new_data;
    v->max = c;

    return 0;
error:
    if (new_data) free(new_data);
    return -1;
}

int Vector_init(Vector* v, const size_t element_size, __destroy_f destroy)
{
    check_ptr(v);

    v->data = malloc(VECTOR_MIN_CAPACITY * element_size);
    check_alloc(v->data);

    v->end = 0;
    v->max = VECTOR_MIN_CAPACITY;
    v->element_size = element_size;
    v->destroy = destroy;

    return 0;
error:
    if (v->data) free(v->data);
    return -1;
}

void Vector_clear(Vector* v)
{
    if (v && v->data) {
        if (v->destroy) {
            for (size_t i = 0; i < v->end; ++i) {
                v->destroy(v->data + i * v->element_size);
            }
        }
        v->end = 0;
        __Vector_resize(v, VECTOR_MIN_CAPACITY);
    }
}

void Vector_destroy(Vector* v)
{
    if (v && v->data) {
        Vector_clear(v);
        free(v->data);
        v->data = NULL;
    }
}

int Vector_get(const Vector *v, const size_t i, void* out)
{
    check_ptr(v);
    check_ptr(out);
    check(i < v->end, "Index out of range: %lu > %lu", i, v->end);

    memmove(out, v->data + i * v->element_size, v->element_size);
    return 0;
error:
    return -1;
}

int Vector_set(Vector* v, const size_t i, const void* in)
{
    check_ptr(v);
    check_ptr(in);
    check(i <= v->end, "Index out of range: %lu > %lu", i, v->end);

    if (i == v->end) {
        if (v->end == v->max) {
            check(!__Vector_resize(v, v->max << 1), "Failed to expand.");
        }
        ++v->end;
    } else if (i < v->end && v->destroy) {
        v->destroy(v->data + i * v->element_size);
    }

    memmove(v->data + i * v->element_size, in, v->element_size);

    return 0;
error:
    return -1;
}

int Vector_insert(Vector* v, const size_t i, const void* in)
{
    check_ptr(v);
    check_ptr(in);
    check(i <= v->end, "Index out of range: %lu > %lu", i, v->end);

    if (v->end >= v->max) {
        check(!__Vector_resize(v, v->max << 1), "Failed to expand.");
    }

    if (i < v->end) {
        memmove(v->data + (i + 1) * v->element_size,
                v->data + i * v->element_size,
                (v->end - i) * v->element_size);
    }

    memmove(v->data + i * v->element_size, in, v->element_size);

    ++v->end;

    return 0;
error:
    return -1;
}

int Vector_delete(Vector* v, const size_t i)
{
    check_ptr(v);
    check(i < v->end, "Index out of range: %lu > %lu", i, v->end);

    if (v->destroy) {
        v->destroy(v->data + i * v->element_size);
    }

    if (i < v->end - 1) {
        memmove(v->data + i * v->element_size,
                v->data + (i + 1) * v->element_size,
                (v->end - (i + 1)) * v->element_size);
    }

    --v->end;

    if (v->end < v->max / 4 && v->max > VECTOR_MIN_CAPACITY) {
        size_t new_size = v->max >> 1;
        check(!__Vector_resize(v, new_size >= VECTOR_MIN_CAPACITY
                                  ? new_size
                                  : VECTOR_MIN_CAPACITY),
                "Failed to contract.");
    }

    return 0;
error:
    return -1;
}

int Vector_push_back(Vector* v, const void* in)
{
    return Vector_set(v, v->end, in);
}

int Vector_pop_back(Vector* v, void* out)
{
    check_ptr(v);
    check(v->end > 0, "Attempt to pop_back from empty vector.");

    if (out) {
        check(!Vector_get(v, v->end - 1, out), "Failed to get element.");
    }

    check(!Vector_delete(v, v->end - 1), "Failed to delete element.");

    return 0;
error:
    return -1;
}
