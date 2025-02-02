/*************************************************************************************************
 *
 * type_interface.h
 *
 * Type interfaces are the means whereby the containers in the library can handle different types
 * generically. A type interface is just a struct containing the type's size and function pointers
 * for copy, move, swap, destruction, comparison, hash and print operations. Each of the
 * containers takes one or more pointers to type interfaces as arguments for initialization and
 * uses them to perform the required actions on its contents. Not all fields in a type interface
 * need to be present for all containers. If move/copy/destruction operations are not defined,
 * the operations will only handle the top level data of the type, ignoring possible pointers to
 * sub-data.
 *
 ************************************************************************************************/

#ifndef _type_interface_h
#define _type_interface_h

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

typedef void        (*copy_f)       (void *dest, const void *src);
typedef void        (*move_f)       (void *dest, void *src);
typedef void        (*swap_f)       (void *a, void *b);
typedef void        (*destroy_f)    (void *obj);
typedef int         (*compare_f)    (const void *a, const void *b);
typedef uint32_t    (*hash_f)       (const void *obj);
typedef void        (*print_f)      (FILE *stream, const void *obj);

typedef struct t_intf {
    size_t      size;
    copy_f      copy;
    move_f      move;
    swap_f      swap;
    destroy_f   destroy;
    compare_f   compare;
    hash_f      hash;
    print_f     print;
} t_intf;

void *      t_allocate  (const t_intf *T, size_t n);
void        t_copy      (const t_intf *T, void *dest, const void *src);
void        t_move      (const t_intf *T, void *dest, void *src);
int         t_swap      (const t_intf *T, void *a, void *b);
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

#endif /* _type_interface_h */
