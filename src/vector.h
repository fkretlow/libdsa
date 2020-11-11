#ifndef _vector_h
#define _vector_h

#include <stdlib.h>
#include "type_interface.h"

#define VECTOR_MIN_CAPACITY 8lu

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
    TypeInterface *element_type;
} Vector;

#define Vector_capacity(V) (V)->capacity
#define Vector_size(V) (V)->size
#define Vector_empty(V) ((V)->size == 0)

int Vector_initialize(Vector *V, TypeInterface *element_type);
void Vector_destroy(Vector *V);
Vector *Vector_new(TypeInterface *element_type);
void Vector_delete(Vector *V);

int Vector_reserve(Vector *V, const size_t capacity);
int Vector_shrink_to_fit(Vector *V);
void Vector_clear(Vector *V);
int Vector_get(const Vector *V, const size_t i, void *out);
int Vector_set(Vector *V, const size_t i, const void *in);
int Vector_insert(Vector *V, const size_t i, const void *in);
int Vector_remove(Vector *V, const size_t i);
int Vector_push_back(Vector *V, const void *in);
int Vector_pop_back(Vector *V, void *out);

#endif // _vector_h
