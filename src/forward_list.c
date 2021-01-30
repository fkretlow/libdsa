/*************************************************************************************************
 *
 * forward_list.c
 *
 * Implementation of the list interface defined in flist.h.
 * No data field is defined in the node header struct `flist_n`, but on construction enough
 * memory is allocated for the node header and one data object according to the type interface
 * stored in the list header. This allows for generic handling of arbitrary types while reducing
 * memory overhead and indirection.
 *
 * Author: Florian Kretlow, 2021
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include "check.h"
#include "forward_list.h"

/* static inline int flist_invariant(const flist *L) -- Health check. */
static inline int flist_invariant(const flist *L)
{
    if (L->front) check(L->count > 0, "list invariant violated: L->front && L->count == 0");
    return 0;
error:
    return -1;
}

#define flist_n_size(L) (sizeof(flist_n) + t_size((L)->data_type))

/* static inline flist_n *flist_n_new(const flist *L, const void *v)
 * Create a new node with the value v. Return a pointer to it or NULL on error. */
static inline flist_n *flist_n_new(const flist *L, const void *v)
{
    assert(L && L->data_type && v);
    size_t size = flist_n_size(L);
    assert(size > sizeof(flist_n));

    flist_n *n = calloc(1, size);
    check_alloc(n);

    t_copy(L->data_type, flist_n_data(n), v);
    n->has_data = 1;

    return n;
error:
    /* The last check happens before the data is copied, no need to destroy it. */
    return NULL;
}

/* static inline void flist_n_delete(const flist *L, flist_n *n)
 * Delete n, freeing any associated memory. */
static inline void flist_n_delete(const flist *L, flist_n *n)
{
    assert(L && L->data_type && n);
    if (n->has_data) t_destroy(L->data_type, flist_n_data(n));
    free(n);
}

/* static void flist_n_set(const flist *L, flist_n *n, const void *v)
 * Set the payload of n to v, destroying any existing payload. */
static void flist_n_set(const flist *L, flist_n *n, const void *v)
{
    assert(L && L->data_type && n && v);

    if (n->has_data) t_destroy(L->data_type, flist_n_data(n));
    t_copy(L->data_type, flist_n_data(n), v);
    n->has_data = 1;
}

/* int      flist_initialize(flist *L, t_intf *t)
 * flist *  flist_new       (          t_intf *t)
 * flist_initialize initializes an flist at the address pointed to by L (assuming there's enough
 * space), and returns 0 on success or -1 on error. flist_new allocates and initializes a new
 * flist and returns a pointer to it or NULL on error. */
int flist_initialize(flist *L, t_intf *dt)
{
    check_ptr(dt);

    L->front = NULL;
    L->count = 0;
    L->data_type = dt;

    assert(flist_invariant(L) == 0);
    return 0;
error:
    return -1;
}

flist *flist_new(t_intf *dt)
{
    flist *L = malloc(sizeof(*L));
    check_alloc(L);

    int rc = flist_initialize(L, dt);
    check(rc == 0, "failed to initialize new flist");

    return L;
error:
    if (L) free(L);
    return NULL;
}

/* void flist_clear(flist *L)
 * Delete all nodes, freeing associated memory, and reset L. */
void flist_clear(flist *L)
{
    if (L) {
        assert(flist_invariant(L) == 0);

        flist_n *cur;
        flist_n *next;

        for (cur = L->front; cur != NULL; cur = next) {
            next = cur->next;
            flist_n_delete(L, cur);
        }
        L->front = NULL;
        L->count = 0;
    }
}

/* void flist_destroy(bst *L)
 * void flist_delete (bst *L)
 * Destroy L, freeing any associated memory. flist_delete also calls free on L. */
void flist_destroy(flist *L)
{
    if (L) {
        if (flist_count(L)) flist_clear(L);
        L->data_type = NULL;
    }
}

void flist_delete(flist *L)
{
    if (L) {
        if (flist_count(L)) flist_clear(L);
        free(L);
    }
}

/* static inline flist_n *flist_get_node(const flist *L, size_t i)
 * Returns the node at index i, assuming i is not out of range. */
static inline flist_n *flist_get_node(const flist *L, size_t i)
{
    flist_n *cur = L->front;
    for ( ; i >= 1; --i) cur = cur->next;
    return cur;
}

/* void *flist_get(flist *L, const size_t i)
 * Get a pointer to the data element at index i, or NULL on error or if i is out of range. */
void *flist_get(flist *L, const size_t i)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);

    if (i >= L->count) return NULL;

    flist_n *n = flist_get_node(L, i);
    return flist_n_data(n);
error:
    return NULL;
}

/* int flist_set(flist *L, const size_t i, const void *v)
 * Set the element at index i to v. Returns 0 on success or -1 on error. */
int flist_set(flist *L, const size_t i, const void *v)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);
    check(i < flist_count(L), "index error");
    check_ptr(v);

    flist_n *n = flist_get_node(L, i);
    flist_n_set(L, n, v);

    return 0;
error:
    return -1;
}

/* int flist_insert(flist *L, const size_t i, const void *v)
 * Insert v into the forward list at index i (an element at i must exist). Returns 1 on success or
 * -1 on error. */
int flist_insert(flist *L, const size_t i, const void *v)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);
    check(i < flist_count(L) || i == 0, "index error");
    check_ptr(v);

    flist_n *n = flist_n_new(L, v);
    check(n != NULL, "failed to make new node");

    if (i == 0) {
        n->next = L->front;
        L->front = n;
    } else {
        flist_n *prev = flist_get_node(L, i-1);
        flist_n *next = prev->next;
        prev->next = n;
        n->next = next;
    }
    ++L->count;

    assert(flist_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int flist_remove(flist *L, const size_t i)
 * Remove the element at index i (must exist), shifting all subsequent elements to the left.
 * Returns 1 on success or -1 on error. */
int flist_remove(flist *L, const size_t i)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);
    check(i < flist_count(L), "index error");

    flist_n *n;

    if (i == 0) {
        n = L->front;
        L->front = n->next;
    } else {
        flist_n *prev = flist_get_node(L, i-1);
        n = prev->next;
        prev->next = n->next;
    }

    --L->count;
    flist_n_delete(L, n);

    assert(flist_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int flist_push_front(flist *L, const void *v)
 * Add v at the end. Returns 1 on success or -1 on error. (flist_push_front is defined as a macro
 * in terms of flist_insert.) */
int flist_push_front(flist *L, const void *v)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);
    check_ptr(v);

    flist_n *n = flist_n_new(L, v);
    check(n != NULL, "failed to make new node");

    n->next = L->front;
    L->front = n;
    ++L->count;

    assert(flist_invariant(L) == 0);
    return 1;
error:
    return -1;
}

/* int flist_pop_front  (flist *L, void *out)
 * Remove the first element, moving it to out if out is given. Returns 1 if an element was
 * removed, 0 if the flist was empty, or -1 on error. */
int flist_pop_front(flist *L, void *out)
{
    check_ptr(L);
    assert(flist_invariant(L) == 0);

    if (L->count == 0) return 0;

    flist_n *n = L->front;
    L->front = n->next;
    --L->count;

    if (out) {
        t_move(L->data_type, out, flist_n_data(n));
        n->has_data = 0;
    }

    flist_n_delete(L, n);

    assert(flist_invariant(L) == 0);
    return 1;
error:
    return -1;
}
