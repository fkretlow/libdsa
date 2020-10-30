#ifndef _type_interface_h
#define _type_interface_h

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "hash.h"

typedef void (*copy_f)(void *dest, const void *src);
typedef void (*destroy_f)(void *obj);
typedef int (*compare_f)(const void *a, const void *b);
typedef unsigned long (*hash_f)(const void *obj);

typedef struct TypeInterface {
    size_t size;
    copy_f copy;
    destroy_f destroy;
    compare_f compare;
    hash_f hash;
} TypeInterface;

inline void *TypeInterface_allocate(TypeInterface *T, size_t n)
{
    void *obj = malloc(n * T->size);
    check_alloc(obj);
    return obj;
error:
    return NULL;
}

inline void TypeInterface_copy(TypeInterface *T, void *dest, const void *src)
{
    if (T->copy) {
        T->copy(dest, src);
    } else {
        memmove(dest, src, T->size);
    }
}

inline void TypeInterface_destroy(TypeInterface *T, void *obj)
{
    if (T->destroy) T->destroy(obj);
}

inline int TypeInterface_compare(TypeInterface *T, const void *a, const void *b)
{
    return T->compare(a, b);
}

inline unsigned long TypeInterface_hash(TypeInterface *T, const void *obj)
{
    if (T->hash) {
        return T->hash(obj);
    } else {
        return jenkins_hash(obj, T->size);
    }
}

#define TypeInterface_size(T) (T)->size

#endif // _type_interface_h
