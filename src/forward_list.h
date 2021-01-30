/*************************************************************************************************
 *
 * flist.h
 *
 * A singly-linked list that supports arbitrary data types by way of type interface structs.
 *
 * Author: Florian Kretlow, 2021
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _forward_list_h
#define _forward_list_h

#include <stdlib.h>
#include <string.h>
#include "type_interface.h"

struct flist_n;
typedef struct flist_n {
    struct flist_n *next;
    unsigned char has_data : 1;
} flist_n;

typedef struct flist {
    flist_n *front;
    size_t count;
    t_intf *data_type;
} flist;

#define flist_n_data(n)   (void*)((char*)n + sizeof(flist_n))

#define flist_front(L)   ((L)->front ? flist_n_data((L)->front) : NULL)
#define flist_count(L)   (L)->count
#define flist_empty(L)   ((L)->count == 0)

int     flist_initialize     (flist *L, t_intf *t);
flist * flist_new            (t_intf *data_type);
void    flist_destroy        (flist *L);
void    flist_delete         (flist *L);
void    flist_clear          (flist *L);

void *  flist_get            (flist *L, const size_t i);
int     flist_set            (flist *L, const size_t i, const void *v);
int     flist_insert         (flist *L, const size_t i, const void *v);
int     flist_remove         (flist *L, const size_t i);
int     flist_push_front     (flist *L, const void *v);
int     flist_pop_front      (flist *L, void *out);

#endif /* _forward_list_h */
