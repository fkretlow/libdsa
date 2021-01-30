/*************************************************************************************************
 *
 * list.h
 *
 * A totally normal doubly-linked list that supports arbitrary data types by way of type interface
 * structs.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _list_h
#define _list_h

#include <stdlib.h>
#include <string.h>

#include "type_interface.h"

struct list_n;
typedef struct list_n {
    struct list_n *prev;
    struct list_n *next;
    unsigned char has_data : 1;
} list_n;

typedef struct list {
    list_n *first;
    list_n *last;
    size_t count;
    t_intf *data_type;
} list;

#define list_n_data(n)   (void*)((char*)n + sizeof(list_n))

#define list_first(L)   ((L)->first ? list_n_data((L)->first) : NULL)
#define list_last(L)    ((L)->last  ? list_n_data((L)->last)  : NULL)
#define list_count(L)   (L)->count
#define list_empty(L)   ((L)->count == 0)

int     list_initialize     (list *L, t_intf *t);
list *  list_new            (t_intf *dt);
void    list_destroy        (list *L);
void    list_delete         (list *L);
void    list_clear          (list *L);

void *  list_get            (list *L, const size_t i);
int     list_set            (list *L, const size_t i, const void *v);
int     list_insert         (list *L, const size_t i, const void *v);
int     list_remove         (list *L, const size_t i);
#define list_push_front(l, v) list_insert((l), 0, (v))
int     list_push_back      (list *L, const void *v);
int     list_pop_front      (list *L, void *out);
int     list_pop_back       (list *L, void *out);


list_n* __N;
#define list_foreach(L, D) \
    for (__N = (L)->first, D = list_n_data(__N); \
            __N != NULL; __N = __N->next, D = __N ? list_n_data(__N) : NULL)

#endif /* _list_h */
