#include <assert.h>
#include "check.h"
#include "list.h"

static inline int list_invariant(const list *L)
{
    if (L->first) check(L->last, "list invariant violated: L->first && !L->last");
    if (L->last) check(L->first, "list invariant violated: !L->first && L->last");
    if (L->first) check(L->count > 0, "list invariant violated: L->first && L->count == 0");
    return 0;
error:
    return -1;
}

static inline listn *listn_new(void)
{
    listn *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void listn_delete(const list *L, listn *n)
{
    if (n) {
        if (n->data && L) {
            t_destroy(L->element_type, n->data);
        }
        free(n->data);
        free(n);
    }
}

static int listn_set(const list *L, listn *n, const void *in)
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

static inline int listn_get(const list *L, listn *n, void *out)
{
    check_ptr(L);
    check_ptr(n);
    check_ptr(out);

    t_copy(L->element_type, out, n->data);

    return 0;
error:
    return -1;
}

int list_initialize(list *L, t_intf *element_type)
{
    check_ptr(element_type);

    L->first = L->last = NULL;
    L->count = 0;
    L->element_type = element_type;

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

list *list_new(t_intf *element_type)
{
    list *L = malloc(sizeof(*L));
    check_alloc(L);

    int rc = list_initialize(L, element_type);
    check(rc == 0, "Failed to initialize new list.");

    return L;
error:
    if (L) free(L);
    return NULL;
}

void list_delete(list *L)
{
    if (L) {
        if (L->count) list_clear(L);
        free(L);
    }
}

void list_clear(list *L)
{
    if (L) {
        assert(!list_invariant(L));

        listn *cur;
        listn *next;

        for (cur = L->first; cur != NULL; cur = next) {
            next = cur->next;
            listn_delete(L, cur);
        }
        L->first = L->last = NULL;
        L->count = 0;
    }
}

static listn *list_get_node(const list *L, size_t i)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check(i < L->count, "Index out of range: %lu >= %lu", i, L->count);

    listn *cur = L->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    return cur;
error:
    return NULL;
}

int list_get(const list *L, const size_t i, void *out)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(out);

    listn *n = list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    check(!listn_get(L, n, out), "Failed to hand out data.");

    return 0;
error:
    return -1;
}

int list_set(list *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(in);

    listn *n = list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    check(!listn_set(L, n, in), "Failed to write data to new node.");

    return 0;
error:
    return -1;
}

int list_insert(list *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(in);

    listn *n = listn_new();
    check(n != NULL, "Failed to make new node.");
    check(!listn_set(L, n, in), "Failed to write data to new node.");

    if (i == 0) {
        n->next = L->first;
        if (L->first) L->first->prev = n;
        L->first = n;
        if (L->count == 0) L->last = n;
    } else {
        listn *next = list_get_node(L, i);
        check(next != NULL, "Failed to get node at index %lu.", i);
        listn *prev = next->prev;

        prev->next = n;
        n->prev = prev;
        next->prev = n;
        n->next = next;
    }
    ++L->count;

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

int list_remove(list *L, const size_t i)
{
    check_ptr(L);
    assert(!list_invariant(L));

    listn *n = list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);

    if (i == 0) {
        assert(n->prev == NULL && "n->prev != NULL at index 0.");
        if (n->next) {
            L->first = n->next;
            L->first->prev = NULL;
        } else {
            L->first = L->last = NULL;
        }
    } else if (i == L->count - 1) {
        assert(n->next == NULL && "n->next != NULL at index L->count - 1.");
        L->last = n->prev;
        L->last->next = NULL;
    } else {
        listn *next = n->next;
        listn *prev = n->prev;
        prev->next = next;
        next->prev = prev;
    }
    --L->count;
    listn_delete(L, n);

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

int list_push_back(list *L, const void *in)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(in);

    listn *n = listn_new();
    check(n != NULL, "Failed to make new node.");
    check(!listn_set(L, n, in), "Failed to write data to new node.");

    if (L->count == 0) {
        L->first = L->last = n;
    } else {
        L->last->next = n;
        n->prev = L->last;
        L->last = n;
    }
    ++L->count;

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

int list_pop_front(list *L, void *out)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(out);
    check(L->count > 0, "Attempt to pop from empty list.");

    listn *n = L->first;
    check(!listn_get(L, n, out), "Failed to hand out data.");
    check(!list_remove(L, 0), "Failed to remove first node.");

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

int list_pop_back(list *L, void *out)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(out);
    check(L->count > 0, "Attempt to pop from empty list.");

    listn *n = L->last;
    check(!listn_get(L, n, out), "Failed to hand out data.");
    check(!list_remove(L, L->count - 1), "Failed to remove last node.");

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}
