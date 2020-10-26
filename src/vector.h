#ifndef _vector_h
#define _vector_h

#include <stdlib.h>
#include "container_tools.h"

#define VECTOR_MIN_CAPACITY 8lu

typedef struct _vector {
    char *data;
    size_t size;
    size_t capacity;
    size_t element_size;
    copy_f copy_element;
    destroy_f destroy_element;
} _vector;

typedef _vector *Vector;

#define Vector_capacity(V) (V)->capacity
#define Vector_size(V) (V)->size
#define Vector_empty(V) ((V)->size == 0)

int _vector_init(_vector *V,
                 const size_t element_size,
                 copy_f copy_element,
                 destroy_f destroy_element);
void _vector_dealloc(_vector *V);

Vector Vector_new(const size_t element_size,
                  copy_f copy_element,
                  destroy_f destroy_element);
void Vector_delete(Vector V);
int Vector_reserve(Vector V, const size_t capacity);
int Vector_shrink_to_fit(Vector V);
void Vector_clear(Vector V);
int Vector_get(const Vector V, const size_t i, void *out);
int Vector_set(Vector V, const size_t i, const void *in);
int Vector_insert(Vector V, const size_t i, const void *in);
int Vector_remove(Vector V, const size_t i);
int Vector_push_back(Vector V, const void *in);
int Vector_pop_back(Vector V, void *out);

#endif // _vector_h
