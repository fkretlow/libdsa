#include <assert.h>
#include "list.h"

static inline int List_invariant(const List *L)
{
    if (L->first) check(L->last, "List invariant violated: L->first && !L->last");
    if (L->last) check(L->first, "List invariant violated: !L->first && L->last");
    if (L->first) check(L->size > 0, "List invariant violated: L->first && L->size == 0");
    return 0;
error:
    return -1;
}

static inline ListNode *ListNode_new(void)
{
    ListNode *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void ListNode_delete(const List *L, ListNode *n)
{
    if (n) {
        if (n->data && L) {
            t_destroy(L->element_type, n->data);
        }
        free(n->data);
        free(n);
    }
}

static int ListNode_set(const List *L, ListNode *n, const void *in)
{
    check_ptr(L);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        t_destroy(L->element_type, n->data);
    } else {
        n->data = t_allocate(L->element_type, 1);
        check(n->data != NULL, "Failed to allocate memory for new element.");
    }

    t_copy(L->element_type, n->data, in);

    return 0;
error:
    return -1;
}

static inline int ListNode_get(const List *L, ListNode *n, void *out)
{
    check_ptr(L);
    check_ptr(n);
    check_ptr(out);

    t_copy(L->element_type, out, n->data);

    return 0;
error:
    return -1;
}

int List_initialize(List *L, t_intf *element_type)
{
    check_ptr(element_type);

    L->first = L->last = NULL;
    L->size = 0;
    L->element_type = element_type;

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}

List *List_new(t_intf *element_type)
{
    List *L = malloc(sizeof(*L));
    check_alloc(L);

    int rc = List_initialize(L, element_type);
    check(rc == 0, "Failed to initialize new list.");

    return L;
error:
    if (L) free(L);
    return NULL;
}

void List_delete(List *L)
{
    if (L) {
        if (L->size) List_clear(L);
        free(L);
    }
}

void List_clear(List *L)
{
    if (L) {
        assert(!List_invariant(L));

        ListNode *cur;
        ListNode *next;

        for (cur = L->first; cur != NULL; cur = next) {
            next = cur->next;
            ListNode_delete(L, cur);
        }
        L->first = L->last = NULL;
        L->size = 0;
    }
}

static ListNode *List_get_node(const List *L, size_t i)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check(i < L->size, "Index out of range: %lu >= %lu", i, L->size);

    ListNode *cur = L->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    return cur;
error:
    return NULL;
}

int List_get(const List *L, const size_t i, void *out)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(out);

    ListNode *n = List_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    check(!ListNode_get(L, n, out), "Failed to hand out data.");

    return 0;
error:
    return -1;
}

int List_set(List *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(in);

    ListNode *n = List_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    check(!ListNode_set(L, n, in), "Failed to write data to new node.");

    return 0;
error:
    return -1;
}

int List_insert(List *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(in);

    ListNode *n = ListNode_new();
    check(n != NULL, "Failed to make new node.");
    check(!ListNode_set(L, n, in), "Failed to write data to new node.");

    if (i == 0) {
        n->next = L->first;
        if (L->first) L->first->prev = n;
        L->first = n;
        if (L->size == 0) L->last = n;
    } else {
        ListNode *next = List_get_node(L, i);
        check(next != NULL, "Failed to get node at index %lu.", i);
        ListNode *prev = next->prev;

        prev->next = n;
        n->prev = prev;
        next->prev = n;
        n->next = next;
    }
    ++L->size;

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}

int List_remove(List *L, const size_t i)
{
    check_ptr(L);
    assert(!List_invariant(L));

    ListNode *n = List_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);

    if (i == 0) {
        assert(n->prev == NULL && "n->prev != NULL at index 0.");
        if (n->next) {
            L->first = n->next;
            L->first->prev = NULL;
        } else {
            L->first = L->last = NULL;
        }
    } else if (i == L->size - 1) {
        assert(n->next == NULL && "n->next != NULL at index L->size - 1.");
        L->last = n->prev;
        L->last->next = NULL;
    } else {
        ListNode *next = n->next;
        ListNode *prev = n->prev;
        prev->next = next;
        next->prev = prev;
    }
    --L->size;
    ListNode_delete(L, n);

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}

int List_push_back(List *L, const void *in)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(in);

    ListNode *n = ListNode_new();
    check(n != NULL, "Failed to make new node.");
    check(!ListNode_set(L, n, in), "Failed to write data to new node.");

    if (L->size == 0) {
        L->first = L->last = n;
    } else {
        L->last->next = n;
        n->prev = L->last;
        L->last = n;
    }
    ++L->size;

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}

int List_pop_front(List *L, void *out)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(out);
    check(L->size > 0, "Attempt to pop from empty list.");

    ListNode *n = L->first;
    check(!ListNode_get(L, n, out), "Failed to hand out data.");
    check(!List_remove(L, 0), "Failed to remove first node.");

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}

int List_pop_back(List *L, void *out)
{
    check_ptr(L);
    assert(!List_invariant(L));
    check_ptr(out);
    check(L->size > 0, "Attempt to pop from empty list.");

    ListNode *n = L->last;
    check(!ListNode_get(L, n, out), "Failed to hand out data.");
    check(!List_remove(L, L->size - 1), "Failed to remove last node.");

    assert(!List_invariant(L));
    return 0;
error:
    return -1;
}
