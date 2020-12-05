#ifndef _vector_h
#define _vector_h

#include <stdlib.h>
#include "type_interface.h"

#define VECTOR_MIN_CAPACITY 8lu

typedef struct {
    char *      data;
    size_t      count;
    size_t      capacity;
    t_intf *    data_type;
} vector;

#define vector_capacity(V)  (V)->capacity
#define vector_count(V)     (V)->count
#define vector_empty(V)     ((V)->count == 0)

#define vector_get(V, i) \
    (i < ((V)->count) ? (void*)((V)->data + (i) * t_size((V)->data_type)) : NULL)
#define vector_first(V) \
    ((V)->count > 0 ? (void*)(V)->data : NULL)
#define vector_last(V) \
    ((V)->count > 0 ? (void*)((V)->data + ((V)->count - 1) * t_size((V)->data_type)) : NULL)

int         vector_initialize       (vector *V, t_intf *dt);
vector *    vector_new              (           t_intf *dt);
void        vector_destroy          (vector *V);
void        vector_delete           (vector *V);

int         vector_reserve          (vector *V, const size_t n);
int         vector_shrink_to_fit    (vector *V);
void        vector_clear            (vector *V);
int         vector_set              (vector *V, const size_t i, const void *e);
int         vector_insert           (vector *V, const size_t i, const void *e);
int         vector_remove           (vector *V, const size_t i);
int         vector_push_back        (vector *V, const void *e);
int         vector_pop_back         (vector *V, void *out);

#endif // _vector_h
