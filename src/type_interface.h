#ifndef _type_interface_h
#define _type_interface_h

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

typedef void        (*copy_f)       (void *dest, const void *src);
typedef void        (*destroy_f)    (void *obj);
typedef int         (*compare_f)    (const void *a, const void *b);
typedef uint32_t    (*hash_f)       (const void *obj);
typedef void        (*print_f)      (FILE *stream, const void *obj);

typedef struct t_intf {
    size_t size;
    copy_f copy;
    destroy_f destroy;
    compare_f compare;
    hash_f hash;
    print_f print;
} t_intf;

void *      t_allocate  (const t_intf *T, size_t n);
void        t_copy      (const t_intf *T, void *dest, const void *src);
void        t_destroy   (const t_intf *T, void *obj);
int         t_compare   (const t_intf *T, const void *a, const void *b);
uint32_t    t_hash      (const t_intf *T, const void *obj);
void        t_print     (const t_intf *T, FILE *stream, const void *obj);

#define t_size(T) (T)->size

/* Predefined type interfaces */
t_intf str_type;

t_intf int_type;
int         int_compare (const void *a, const void *b);
uint32_t    int_hash    (const void *i);
void        int_print   (FILE *stream, const void *i);

t_intf pointer_type;

#endif // _type_interface_h
