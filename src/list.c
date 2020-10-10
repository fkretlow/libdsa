#include <assert.h>

#include "list.h"

static int __ListNode_new(__ListNode** node_out)
{
    __ListNode* new = calloc(1, sizeof(*new));
    check_alloc(new);
    *node_out = new;
    return 0;
error:
    return -1;
}

static void __ListNode_delete(const List* l, __ListNode* n)
{
    if (n) {
        if (n->data && l && l->destroy) {
            l->destroy(n->data);
        }
    }
    free(n->data);
    free(n);
}

static int __ListNode_set(const List* l, __ListNode* n, const void* in)
{
    check_ptr(l);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        if (l->destroy) {
            l->destroy(n->data);
        }
    } else {
        n->data = malloc(l->element_size);
        check_alloc(n->data);
    }
    memmove(n->data, in, l->element_size);

    return 0;
error:
    return -1;
}

int List_init(List* l, const size_t element_size, __destroy_f destroy)
{
    check_ptr(l);

    l->first = l->last = NULL;
    l->element_size = element_size;
    l->size = 0;
    l->destroy = destroy;

    return 0;
error:
    return -1;
}

void List_clear(List* l)
{
    __ListNode* cur;
    __ListNode* next;

    for (cur = l->first; cur != NULL; cur = next) {
        next = cur->next;
        __ListNode_delete(l, cur);
    }
    l->size = 0;
}

static int __List_get_node(const List* l, size_t i, __ListNode** node_out)
{
    check_ptr(l);
    check_ptr(node_out);
    check(i < l->size, "Index out of range: %lu >= %lu", i, l->size);

    __ListNode* cur = l->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    *node_out = cur;
    return 0;
error:
    return -1;
}

int List_get(const List* l, const size_t i, void* out)
{
    check_ptr(l);
    check_ptr(out);

    __ListNode* n;
    check(!__List_get_node(l, i, &n), "Failed to get node at index %lu.", i);
    memmove(out, n->data, l->element_size);

    return 0;
error:
    return -1;
}

int List_set(List* l, const size_t i, const void* in)
{
    check_ptr(l);
    check_ptr(in);

    __ListNode* n;
    check(!__List_get_node(l, i, &n), "Failed to get node at index %lu.", i);
    memmove(n->data, in, l->element_size);

    return 0;
error:
    return -1;
}

int List_insert(List* l, const size_t i, const void* in)
{
    check_ptr(l);
    check_ptr(in);

    __ListNode* new;
    check(!__ListNode_new(&new), "Failed to make new node.");
    check(!__ListNode_set(l, new, in), "Failed to write data to new node.");

    if (i == 0) {
        new->next = l->first;
        if (l->first) l->first->prev = new;
        l->first = new;
    } else {
        __ListNode* next;
        check(!__List_get_node(l, i, &next), "Failed to get node at index %lu.", i);
        __ListNode* prev = next->prev;

        prev->next = new;
        new->prev = prev;
        next->prev = new;
        new->next = next;
    }
    ++l->size;

    return 0;
error:
    return -1;
}

int List_delete(List* l, const size_t i)
{
    check_ptr(l);

    __ListNode *n;
    check(!__List_get_node(l, i, &n), "Failed to get node at index %lu.", i);

    if (i == 0) {
        assert(n->prev == NULL && "n->prev != NULL at index 0.");
        if (n->next) {
            l->first = n->next;
            l->first->prev = NULL;
        } else {
            l->first = l->last = NULL;
        }
    } else if (i == l->size - 1) {
        assert(n->next == NULL && "n->next != NULL at index l->size - 1.");
        l->last = n->prev;
        l->last->next = NULL;
    } else {
        __ListNode* next = n->next;
        __ListNode* prev = n->prev;
        prev->next = next;
        next->prev = prev;
    }
    --l->size;
    __ListNode_delete(l, n);

    return 0;
error:
    return -1;
}

int List_push_back(List* l, const void* in)
{
    check_ptr(l);
    check_ptr(in);

    __ListNode* new;
    check(!__ListNode_new(&new), "Failed to make new node.");
    check(!__ListNode_set(l, new, in), "Failed to write data to new node.");

    if (l->size == 0) {
        l->first = l->last = new;
    } else {
        l->last->next = new;
        new->prev = l->last;
        l->last = new;
    }
    ++l->size;

    return 0;
error:
    return -1;
}

int List_pop_front(List* l, void* out)
{
    check_ptr(l);
    check_ptr(out);
    check(l->size > 0, "Attempt to pop from empty list.");

    memmove(out, l->first->data, l->element_size);
    check(!List_delete(l, 0), "Failed to delete first node.");

    return 0;
error:
    return -1;
}

int List_pop_back(List* l, void* out)
{
    check_ptr(l);
    check_ptr(out);
    check(l->size > 0, "Attempt to pop from empty list.");

    memmove(out, l->last->data, l->element_size);
    check(!List_delete(l, l->size - 1), "Failed to delete last node.");

    return 0;
error:
    return -1;
}

