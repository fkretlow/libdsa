#include <assert.h>
#include "check.h"
#include "list.h"

static inline int list_invariant(const list *L)
{
    if (L->first)   check(L->last,      "list invariant violated: L->first && !L->last");
    if (L->last)    check(L->first,     "list invariant violated: !L->first && L->last");
    if (L->first)   check(L->count > 0, "list invariant violated: L->first && L->count == 0");
    return 0;
error:
    return -1;
}

#define listn_size(L) (sizeof(listn) + t_size((L)->data_type))

static inline listn *listn_new(const list *L)
{
    log_call("L=%p", L);
    assert(L && L->data_type);
    size_t size = listn_size(L);
    assert(size > sizeof(listn));

    listn *n = calloc(1, size);
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void listn_delete(const list *L, listn *n)
{
    log_call("L=%p, n=%p", L, n);
    assert(L && L->data_type && n);
    if (n->has_data) t_destroy(L->data_type, listn_data(n));
    free(n);
}

static void listn_set(const list *L, listn *n, const void *in)
{
    log_call("L=%p, n=%p, in=%p", L, n, in);
    assert(L && L->data_type && n && in);

    if (n->has_data) t_destroy(L->data_type, listn_data(n));
    t_copy(L->data_type, listn_data(n), in);
}

int list_initialize(list *L, t_intf *t)
{
    check_ptr(t);

    L->first = L->last = NULL;
    L->count = 0;
    L->data_type = t;

    assert(!list_invariant(L));
    return 0;
error:
    return -1;
}

list *list_new(t_intf *t)
{
    list *L = malloc(sizeof(*L));
    check_alloc(L);

    int rc = list_initialize(L, t);
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
    check(i < L->count, "index out of range: %lu >= %lu", i, L->count);

    listn *cur = L->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    return cur;
error:
    return NULL;
}

void *list_get(list *L, const size_t i)
{
    check_ptr(L);
    assert(!list_invariant(L));

    if (i >= L->count) return NULL;

    listn *n = list_get_node(L, i);
    check(n != NULL, "failed to get node at index %lu", i);
    return listn_data(n);
error:
    return NULL;
}

int list_set(list *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(in);

    listn *n = list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    listn_set(L, n, in);

    return 0;
error:
    return -1;
}

int list_insert(list *L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!list_invariant(L));
    check_ptr(in);

    listn *n = listn_new(L);
    check(n != NULL, "Failed to make new node.");
    listn_set(L, n, in);

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

    listn *n = listn_new(L);
    check(n != NULL, "Failed to make new node.");
    listn_set(L, n, in);

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

int list_pop_front(list *L)
{
    check_ptr(L);
    assert(!list_invariant(L));

    if (L->count == 0) return 0;

    listn *n = L->first;
    if (n->next) L->first = n->next;
    else         L->first = L->last = NULL;
    --L->count;

    listn_delete(L, n);

    assert(!list_invariant(L));
    return 1;
error:
    return -1;
}

int list_pop_back(list *L)
{
    check_ptr(L);
    assert(!list_invariant(L));

    if (L->count == 0) return 0;

    listn *n = L->last;
    if (n->prev) L->last = n->prev;
    else         L->last = L->first = NULL;
    --L->count;

    listn_delete(L, n);

    assert(!list_invariant(L));
    return 1;
error:
    return -1;
}
