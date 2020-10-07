#ifndef _vector_h
#define _vector_h

#define VECTOR_MIN_CAPACITY 8

typedef struct Vector {
    char* data;
    size_t end;
    size_t max;
    size_t element_size;
    __destroy_f destroy;
} Vector;

#define Vector_capacity(v) (v)->max
#define Vector_size(v) (v)->end
#define Vector_empty(v) ((v)->end == 0)

int Vector_init(Vector* v, __destroy_f destroy);
void Vector_clear(Vector* v);
void Vector_destroy(Vector* v);

int Vector_get(const Vector* v, const size_t i, void* out);
int Vector_set(Vector* v, const size_t i, const void* in);
int Vector_insert(Vector* v, const size_t i, const void *in);
int Vector_delete(Vector* v, const size_t i);
int Vector_push_back(Vector* v, const void* in);
int Vector_pop_back(Vector* v, void* out);

#endif // _vector_h
