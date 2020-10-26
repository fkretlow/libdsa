#include <assert.h>
#include "list.h"

static inline int List_invariant(const List *l)
{
    if (l->first) check(l->last, "List invariant violated: l->first && !l->last");
    if (l->last) check(l->first, "List invariant violated: !l->first && l->last");
    if (l->first) check(l->size > 0, "List invariant violated: l->first && l->size == 0");
    return 0;
error:
    return -1;
}

static inline int ListNode_new(ListNode **node_out)
{
    ListNode *new = calloc(1, sizeof(*new));
    check_alloc(new);
    *node_out = new;
    return 0;
error:
    return -1;
}

static inline void ListNode_delete(const List *l, ListNode *n)
{
    if (n) {
        if (n->data && l && l->destroy) {
            l->destroy(*(void**)(n->data));
        }
    }
    free(n->data);
    free(n);
}

static int ListNode_set(const List *l, ListNode *n, const void *in)
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

    if (l->copy) {
        l->copy(n->data, in);
    } else {
        memmove(n->data, in, l->element_size);
    }

    return 0;
error:
    return -1;
}

int List_init(List *l, const size_t element_size, copy_f copy, destroy_f destroy)
{
    check_ptr(l);

    l->first = l->last = NULL;
    l->element_size = element_size;
    l->size = 0;
    l->copy = copy;
    l->destroy = destroy;

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}

void List_clear(List *l)
{
    assert(!List_invariant(l));

    ListNode *cur;
    ListNode *next;

    for (cur = l->first; cur != NULL; cur = next) {
        next = cur->next;
        ListNode_delete(l, cur);
    }
    l->size = 0;
}

static int List_get_node(const List *l, size_t i, ListNode **node_out)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(node_out);
    check(i < l->size, "Index out of range: %lu >= %lu", i, l->size);

    ListNode *cur = l->first;
    for (size_t j = 0; j < i; ++j) cur = cur->next;

    *node_out = cur;
    return 0;
error:
    return -1;
}

int List_get(const List *l, const size_t i, void *out)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(out);

    ListNode *n;
    check(!List_get_node(l, i, &n), "Failed to get node at index %lu.", i);
    memmove(out, n->data, l->element_size);

    return 0;
error:
    return -1;
}

int List_set(List *l, const size_t i, const void *in)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(in);

    ListNode *n;
    check(!List_get_node(l, i, &n), "Failed to get node at index %lu.", i);
    memmove(n->data, in, l->element_size);

    return 0;
error:
    return -1;
}

int List_insert(List *l, const size_t i, const void *in)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(in);

    ListNode *new;
    check(!ListNode_new(&new), "Failed to make new node.");
    check(!ListNode_set(l, new, in), "Failed to write data to new node.");

    if (i == 0) {
        new->next = l->first;
        if (l->first) l->first->prev = new;
        l->first = new;
        if (l->size == 0) l->last = new;
    } else {
        ListNode *next;
        check(!List_get_node(l, i, &next), "Failed to get node at index %lu.", i);
        ListNode *prev = next->prev;

        prev->next = new;
        new->prev = prev;
        next->prev = new;
        new->next = next;
    }
    ++l->size;

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}

int List_delete(List *l, const size_t i)
{
    check_ptr(l);
    assert(!List_invariant(l));

    ListNode *n;
    check(!List_get_node(l, i, &n), "Failed to get node at index %lu.", i);

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
        ListNode *next = n->next;
        ListNode *prev = n->prev;
        prev->next = next;
        next->prev = prev;
    }
    --l->size;
    ListNode_delete(l, n);

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}

int List_push_back(List *l, const void *in)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(in);

    ListNode *new;
    check(!ListNode_new(&new), "Failed to make new node.");
    check(!ListNode_set(l, new, in), "Failed to write data to new node.");

    if (l->size == 0) {
        l->first = l->last = new;
    } else {
        l->last->next = new;
        new->prev = l->last;
        l->last = new;
    }
    ++l->size;

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}

int List_pop_front(List *l, void *out)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(out);
    check(l->size > 0, "Attempt to pop from empty list.");

    if (l->copy) {
        l->copy(out, l->first->data);
    } else {
        memmove(out, l->first->data, l->element_size);
    }
    check(!List_delete(l, 0), "Failed to delete first node.");

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}

int List_pop_back(List *l, void *out)
{
    check_ptr(l);
    assert(!List_invariant(l));
    check_ptr(out);
    check(l->size > 0, "Attempt to pop from empty list.");

    if (l->copy) {
        l->copy(out, l->last->data);
    } else {
        memmove(out, l->last->data, l->element_size);
    }
    check(!List_delete(l, l->size - 1), "Failed to delete last node.");

    assert(!List_invariant(l));
    return 0;
error:
    return -1;
}
