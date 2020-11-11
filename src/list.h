#ifndef _list_h
#define _list_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "type_interface.h"

struct ListNode;
typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
    char *data;
} ListNode;

typedef struct List {
    ListNode *first;
    ListNode *last;
    size_t size;
    TypeInterface *element_type;
} List;

#define List_first(L) ((L)->first ? (L)->first->data : NULL)
#define List_last(L) ((L)->last ? (void*)((L)->last->data) : NULL)
#define List_size(L) (L)->size
#define List_empty(L) ((L)->size == 0)

int List_initialize(List *L, TypeInterface *element_type);
List *List_new(TypeInterface *element_type);
void List_delete(List *L);
void List_clear(List *L);

int List_get(const List *L, const size_t i, void *out);
int List_set(List *L, const size_t i, const void *in);
int List_insert(List *L, const size_t i, const void *in);
int List_remove(List *L, const size_t i);
#define List_push_front(l, in) List_insert((l), 0, (in))
int List_push_back(List *L, const void *in);
int List_pop_front(List *L, void *out);
int List_pop_back(List *L, void *out);

ListNode* __N;
#define List_foreach(L, D) \
    for (__N = (L)->first, D = (void*)(__N->data); \
            __N != NULL; __N = __N->next, D = __N ? (void*)(__N->data) : NULL)

#endif // _list_ha
