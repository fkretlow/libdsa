#ifndef _list_h
#define _list_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "container_tools.h"

struct __ListNode;
typedef struct __ListNode {
    struct __ListNode *prev;
    struct __ListNode *next;
    char *data;
} __ListNode;

typedef struct List {
    __ListNode *first;
    __ListNode *last;
    __destroy_f destroy;
    size_t element_size;
    size_t size;
} List;

#define List_first(L) ((L)->first ? (L)->first->data : NULL)
#define List_last(L) ((L)->last ? (L)->last->data : NULL)
#define List_size(L) (L)->size
#define List_empty(L) ((L)->size == 0)

int List_init(List *l, const size_t element_size, __destroy_f destroy);
void List_clear(List *l);

int List_get(const List *l, const size_t i, void *out);
int List_set(List *l, const size_t i, const void *in);
int List_insert(List *l, const size_t i, const void *in);
int List_delete(List *l, const size_t i);
#define List_push_front(l, in) List_insert((l), 0, (in))
int List_push_back(List *l, const void *in);
int List_pop_front(List *l, void *out);
int List_pop_back(List *l, void *out);

__ListNode* __N;
#define List_foreach(L, D) \
    for (__N = (L)->first, D = (void*)(__N->data); \
            __N != NULL; __N = __N->next, D = __N ? (void*)(__N->data) : NULL)

#endif // _list_h
