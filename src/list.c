#include <assert.h>
#include "list.h"

static inline int _list_invariant(const List L)
{
    if (L->first) check(L->last, "List invariant violated: L->first && !L->last");
    if (L->last) check(L->first, "List invariant violated: !L->first && L->last");
    if (L->first) check(L->size > 0, "List invariant violated: L->first && L->size == 0");
    return 0;
error:
    return -1;
}

static inline _list_node *_list_node_new(void)
{
    _list_node *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void _list_node_delete(const List L, _list_node *n)
{
    if (n) {
        if (n->data && L && L->destroy) {
                L->destroy(*(void**)n->data);
        }
        free(n->data);
        free(n);
    }
}

static int _list_node_set(const List L, _list_node *n, const void *in)
{
    check_ptr(L);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        if (L->destroy) {
            L->destroy(*(void**)n->data);
        }
    } else {
        n->data = malloc(L->element_size);
        check_alloc(n->data);
    }

    if (L->copy) {
        L->copy(n->data, in);
    } else {
        memmove(n->data, in, L->element_size);
    }

    return 0;
error:
    return -1;
}

List List_new(const size_t element_size, copy_f copy, destroy_f destroy)
{
    List L = malloc(sizeof(*L));
    check_ptr(L);

    L->first = L->last = NULL;
    L->element_size = element_size;
    L->size = 0;
    L->copy = copy;
    L->destroy = destroy;

    assert(!_list_invariant(L));
    return L;
error:
    return NULL;
}

void List_delete(List L)
{
    if (L) {
        if (L->size) List_clear(L);
        free(L);
    }
}

void List_clear(List L)
{
    if (L) {
        assert(!_list_invariant(L));

        _list_node *cur;
        _list_node *next;

        for (cur = L->first; cur != NULL; cur = next) {
            next = cur->next;
            _list_node_delete(L, cur);
        }
        L->first = L->last = NULL;
        L->size = 0;
    }
}

static _list_node *_list_get_node(const List L, size_t i)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check(i < L->size, "Index out of range: %lu >= %lu", i, L->size);

    _list_node *cur = L->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    return cur;
error:
    return NULL;
}

int List_get(const List L, const size_t i, void *out)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(out);

    _list_node *n = _list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    memmove(out, n->data, L->element_size);

    return 0;
error:
    return -1;
}

int List_set(List L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(in);

    _list_node *n = _list_get_node(L, i);
    check(n != NULL, "Failed to get node at index %lu.", i);
    check(!_list_node_set(L, n, in), "Failed to write data to new node.");

    return 0;
error:
    return -1;
}

int List_insert(List L, const size_t i, const void *in)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(in);

    _list_node *n = _list_node_new();
    check(n != NULL, "Failed to make new node.");
    check(!_list_node_set(L, n, in), "Failed to write data to new node.");

    if (i == 0) {
        n->next = L->first;
        if (L->first) L->first->prev = n;
        L->first = n;
        if (L->size == 0) L->last = n;
    } else {
        _list_node *next = _list_get_node(L, i);
        check(next != NULL, "Failed to get node at index %lu.", i);
        _list_node *prev = next->prev;

        prev->next = n;
        n->prev = prev;
        next->prev = n;
        n->next = next;
    }
    ++L->size;

    assert(!_list_invariant(L));
    return 0;
error:
    return -1;
}

int List_remove(List L, const size_t i)
{
    check_ptr(L);
    assert(!_list_invariant(L));

    _list_node *n = _list_get_node(L, i);
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
        _list_node *next = n->next;
        _list_node *prev = n->prev;
        prev->next = next;
        next->prev = prev;
    }
    --L->size;
    _list_node_delete(L, n);

    assert(!_list_invariant(L));
    return 0;
error:
    return -1;
}

int List_push_back(List L, const void *in)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(in);

    _list_node *n = _list_node_new();
    check(n != NULL, "Failed to make new node.");
    check(!_list_node_set(L, n, in), "Failed to write data to new node.");

    if (L->size == 0) {
        L->first = L->last = n;
    } else {
        L->last->next = n;
        n->prev = L->last;
        L->last = n;
    }
    ++L->size;

    assert(!_list_invariant(L));
    return 0;
error:
    return -1;
}

int List_pop_front(List L, void *out)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(out);
    check(L->size > 0, "Attempt to pop from empty list.");

    if (L->copy) {
        L->copy(out, L->first->data);
    } else {
        memmove(out, L->first->data, L->element_size);
    }
    check(!List_remove(L, 0), "Failed to remove first node.");

    assert(!_list_invariant(L));
    return 0;
error:
    return -1;
}

int List_pop_back(List L, void *out)
{
    check_ptr(L);
    assert(!_list_invariant(L));
    check_ptr(out);
    check(L->size > 0, "Attempt to pop from empty list.");

    if (L->copy) {
        L->copy(out, L->last->data);
    } else {
        memmove(out, L->last->data, L->element_size);
    }
    check(!List_remove(L, L->size - 1), "Failed to remove last node.");

    assert(!_list_invariant(L));
    return 0;
error:
    return -1;
}
