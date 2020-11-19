#ifndef _list_h
#define _list_h

#include <stdlib.h>
#include <string.h>

#include "type_interface.h"

struct listn;
typedef struct listn {
    struct listn *prev;
    struct listn *next;
    char *data;
} listn;

typedef struct list {
    listn *first;
    listn *last;
    size_t count;
    t_intf *element_type;
} list;

#define list_first(L)   ((L)->first ? (L)->first->data : NULL)
#define list_last(L)    ((L)->last ? (void*)((L)->last->data) : NULL)
#define list_count(L)    (L)->count
#define list_empty(L)   ((L)->count == 0)

int     list_initialize     (list *L, t_intf *element_type);
list *  list_new            (t_intf *element_type);
void    list_delete         (list *L);
void    list_clear          (list *L);

int     list_get            (const list *L, const size_t i, void *out);
int     list_set            (      list *L, const size_t i, const void *in);
int     list_insert         (      list *L, const size_t i, const void *in);
int     list_remove         (      list *L, const size_t i);
#define list_push_front(l, in) list_insert((l), 0, (in))
int     list_push_back      (      list *L,                 const void *in);
int     list_pop_front      (      list *L,                 void *out);
int     list_pop_back       (      list *L,                 void *out);

listn* __N;
#define list_foreach(L, D) \
    for (__N = (L)->first, D = (void*)(__N->data); \
            __N != NULL; __N = __N->next, D = __N ? (void*)(__N->data) : NULL)

#endif // _list_ha
