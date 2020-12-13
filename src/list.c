/*************************************************************************************************
 *
 * list.c
 *
 * Implementation of the list interface defined in list.h.
 * No data field is defined in the node header struct `listn`, but on construction enough memory
 * is allocated for the node header and one data object according to the type interface stored in
 * the list header. This allows for generic handling of arbitrary types while reducing memory
 * overhead and indirection. Disclaimer: Don't try this at home... ;)
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include "check.h"
#include "list.h"

/* static inline int list_invariant(const list *L) -- Health check. */
static inline int list_invariant(const list *L)
{
    if (L->first)   check(L->last,      "list invariant violated: L->first && !L->last");
    if (L->last)    check(L->first,     "list invariant violated: !L->first && L->last");
    if (L->first)   check(L->count > 0, "list invariant violated: L->first && L->count == 0");
    return 0;
error:
    return -1;
}

/* Get the size of a node, which is the sum of the size of the node header + the size of data
 * objects. */
#define listn_size(L) (sizeof(listn) + t_size((L)->data_type))

/* static inline listn *listn_new(const list *L, const void *v)
 * Create a new list node with the value v. Return a pointer to it or NULL on error. */
static inline listn *listn_new(const list *L, const void *v)
{
    log_call("L=%p", L);
    assert(L && L->data_type && v);
    size_t size = listn_size(L);
    assert(size > sizeof(listn));

    listn *n = calloc(1, size);
    check_alloc(n);

    t_copy(L->data_type, listn_data(n), v);
    n->has_data = 1;

    return n;
error:
    /* The last check happens before the data is copied, no need to destroy it. */
    return NULL;
}

/* static inline void listn_delete(const list *L, listn *n)
 * Delete n, freeing any associated memory. */
static inline void listn_delete(const list *L, listn *n)
{
    log_call("L=%p, n=%p", L, n);
    assert(L && L->data_type && n);
    if (n->has_data) t_destroy(L->data_type, listn_data(n));
    free(n);
}

/* static void listn_set(const list *L, listn *n, const void *v)
 * Set the payload of n to v, destroying any existing payload. */
static void listn_set(const list *L, listn *n, const void *v)
{
    log_call("L=%p, n=%p, v=%p", L, n, v);
    assert(L && L->data_type && n && v);

    if (n->has_data) t_destroy(L->data_type, listn_data(n));
    t_copy(L->data_type, listn_data(n), v);
    n->has_data = 1;
}

/* int      list_initialize(list *L, t_intf *t)
 * list *   list_new       (         t_intf *t)
 * list_initialize initializes a list at the address pointed to by L (assuming there's enough
 * space), and returns 0 on success or -1 on error. list_new allocates and initializes a new list
 * and returns a pointer to it or NULL on error. */
int list_initialize(list *L, t_intf *t)
{
    check_ptr(t);

    L->first = L->last = NULL;
    L->count = 0;
    L->data_type = t;

    assert(list_invariant(L) == 0);
    return 0;
error:
    return -1;
}

list *list_new(t_intf *t)
{
    list *L = malloc(sizeof(*L));
    check_alloc(L);

    int rc = list_initialize(L, t);
    check(rc == 0, "failed to initialize new list");

    return L;
error:
    if (L) free(L);
    return NULL;
}

/* void list_clear(list *L)
 * Delete all nodes, freeing associated memory, and reset L. */
void list_clear(list *L)
{
    if (L) {
        assert(list_invariant(L) == 0);

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

/* void list_destroy(bst *L)
 * void list_delete (bst *L)
 * Destroy L, freeing any associated memory. list_delete also calls free on L. */
void list_destroy(list *L)
{
    if (L) {
        if (list_count(L)) list_clear(L);
        L->data_type = NULL;
    }
}

void list_delete(list *L)
{
    if (L) {
        if (list_count(L)) list_clear(L);
        free(L);
    }
}

/* static inline listn *list_get_node(const list *L, size_t i)
 * Returns the node at index i, assuming i is not out of range. */
static inline listn *list_get_node(const list *L, size_t i)
{
    listn *cur = L->first;
    for ( ; i >= 1; --i) cur = cur->next;
    return cur;
}

/* void *list_get(list *L, const size_t i)
 * Get a pointer to the data element at index i, or NULL on error or if i is out of range. */
void *list_get(list *L, const size_t i)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);

    if (i >= L->count) return NULL;

    listn *n = list_get_node(L, i);
    check(n != NULL, "failed to get node at index %lu", i);
    return listn_data(n);
error:
    return NULL;
}

/* int list_set(list *L, const size_t i, const void *v)
 * Set the element at index i to v. Returns 0 on success or -1 on error. */
int list_set(list *L, const size_t i, const void *v)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);
    check(i < list_count(L), "index error");
    check_ptr(v);

    listn *n = list_get_node(L, i);
    check(n != NULL, "failed to get node at index %lu", i);
    listn_set(L, n, v);

    return 0;
error:
    return -1;
}

/* int list_insert(list *L, const size_t i, const void *v)
 * Insert v into the list at index i (an element at i must exist). Returns 1 on success or -1 on
 * error. */
int list_insert(list *L, const size_t i, const void *v)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);
    check(i < list_count(L) || i == 0, "index error");
    check_ptr(v);

    listn *n = listn_new(L, v);
    check(n != NULL, "failed to make new node");

    if (i == 0) {
        n->next = L->first;
        if (L->first) L->first->prev = n;
        L->first = n;
        if (L->count == 0) L->last = n;
    } else {
        listn *next = list_get_node(L, i);
        check(next != NULL, "failed to get node at index %lu", i);
        listn *prev = next->prev;

        prev->next = n;
        n->prev = prev;
        next->prev = n;
        n->next = next;
    }
    ++L->count;

    assert(list_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int list_remove(list *L, const size_t i)
 * Remove the element at index i (must exist), shifting all subsequent elements to the left.
 * Returns 1 on success or -1 on error. */
int list_remove(list *L, const size_t i)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);
    check(i < list_count(L), "index error");

    listn *n = list_get_node(L, i);
    check(n != NULL, "failed to get node at index %lu", i);

    if (i == 0) {
        assert(n->prev == NULL && "n->prev != NULL at index 0");
        if (n->next) {
            L->first = n->next;
            L->first->prev = NULL;
        } else {
            L->first = L->last = NULL;
        }
    } else if (i == L->count - 1) {
        assert(n->next == NULL && "n->next != NULL at index L->count - 1");
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

    assert(list_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int list_push_back(list *L, const void *v)
 * Add v at the end. Returns 1 on success or -1 on error. (list_push_front is defined as a macro
 * in terms of list_insert.) */
int list_push_back(list *L, const void *v)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);
    check_ptr(v);

    listn *n = listn_new(L, v);
    check(n != NULL, "failed to make new node");

    if (L->count == 0) {
        L->first = L->last = n;
    } else {
        L->last->next = n;
        n->prev = L->last;
        L->last = n;
    }
    ++L->count;

    assert(list_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int list_pop_front  (list *L, void *out)
 * int list_pop_back   (list *L, void *out)
 * Remove the first/last element, moving it to out if out is given. Returns 1 if an element was
 * removed, 0 if the list was empty, or -1 on error. */
int list_pop_front(list *L, void *out)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);

    if (L->count == 0) return 0;

    listn *n = L->first;
    if (n->next) L->first = n->next;
    else         L->first = L->last = NULL;
    --L->count;

    if (out) {
        t_move(L->data_type, out, listn_data(n));
        n->has_data = 0;
    }

    listn_delete(L, n);

    assert(list_invariant(L) == 0);
    return 1;
error:
    return -1;
}

int list_pop_back(list *L, void *out)
{
    check_ptr(L);
    assert(list_invariant(L) == 0);

    if (L->count == 0) return 0;

    listn *n = L->last;
    if (n->prev) L->last = n->prev;
    else         L->last = L->first = NULL;
    --L->count;

    if (out) {
        t_move(L->data_type, out, listn_data(n));
        n->has_data = 0;
    }

    listn_delete(L, n);

    assert(list_invariant(L) == 0);
    return 1;
error:
    return -1;
}
