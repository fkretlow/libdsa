#ifndef _type_interface_h
#define _type_interface_h

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "hash.h"
#include "str.h"

typedef void (*copy_f)(void *dest, const void *src);
typedef void (*destroy_f)(void *obj);
typedef int (*compare_f)(const void *a, const void *b);
typedef unsigned long (*hash_f)(const void *obj);
typedef void (*print_f)(FILE *stream, const void *obj);

typedef struct TypeInterface {
    size_t size;
    copy_f copy;
    destroy_f destroy;
    compare_f compare;
    hash_f hash;
    print_f print;
} TypeInterface;

void *TypeInterface_allocate(TypeInterface *T, size_t n);
void TypeInterface_copy(TypeInterface *T, void *dest, const void *src);
void TypeInterface_destroy(TypeInterface *T, void *obj);
int TypeInterface_compare(TypeInterface *T, const void *a, const void *b);
unsigned long TypeInterface_hash(TypeInterface *T, const void *obj);
void TypeInterface_print(TypeInterface *T, FILE *stream, const void *obj);

#define TypeInterface_size(T) (T)->size

/* Predefined type interfaces: */
TypeInterface String_type;
TypeInterface int_type;
TypeInterface pointer_type;

#endif // _type_interface_h
