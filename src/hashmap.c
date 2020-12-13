/*************************************************************************************************
 *
 * hashmap.c
 *
 * Implementation of the unordered associative array abstraction, using hashing to distribute keys
 * over linked lists of key-value pairs. Arbitrary key/value types are supported by way of type
 * interface structs.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include <string.h>

#include "check.h"
#include "hashmap.h"

#define hashmapn_data_size(M)   (t_size((M)->key_type) + t_size((M)->value_type))
#define hashmapn_size(M)        (sizeof(hashmapn) + hashmapn_data_size(M))
#define hashmapn_key(M, n)      ((void*)(((char *)(n)) + sizeof(hashmapn)))
#define hashmapn_value(M, n)    ((void*)((char *)(n)) + sizeof(hashmapn) + t_size((M)->key_type))


/* static inline hashmapn *hashmapn_new(const hashmap *M, const void *k, const void *v)
 * Create a new node on the heap, copy k and v into it, and return a pointer to it or NULL on
 * error. Both k and v must be given, because a hashmap entry without either doesn't make sense.
 * */
static inline hashmapn *hashmapn_new(const hashmap *M, const void *k, const void *v)
{
    assert(M && M->key_type && k && M->value_type && v);
    size_t size = hashmapn_size(M);
    hashmapn *n = calloc(1, size);
    check_alloc(n);

    t_copy(M->key_type,   hashmapn_key(M, n),   k);
    t_copy(M->value_type, hashmapn_value(M, n), v);

    return n;
error:
    return NULL;
}

/* static inline void hashmapn_delete(const hashmap *M, hashmapn *n)
 * Delete n, destroying stored data and freeing all associated memory. */
static inline void hashmapn_delete(const hashmap *M, hashmapn *n)
{
    if (n) {
        t_destroy(M->key_type,   hashmapn_key(M, n));
        t_destroy(M->value_type, hashmapn_value(M, n));
        free(n);
    }
}

/* static inline void hashmapn_set_value(const hashmap *M, hashmapn *n, const void *v)
 * Set the value of the node n to v. We assume that no node is ever created without a value, so
 * there is a previous value that we need to destroy. */
static inline void hashmapn_set_value(const hashmap *M, hashmapn *n, const void *v)
{
    assert(M && n && v);
    t_destroy(M->value_type, hashmapn_value(M, n));
    t_copy(M->value_type, hashmapn_value(M, n), v);
}

/* int      hashmap_initialize(hashmap *M, t_intf *kt, t_intf *vt)
 * hashmap *hashmap_new       (            t_intf *kt, t_intf *vt)
 * hashmap_initialize initializes a hashmap at the address pointed to by M (assuming there's
 * sufficient space). hashmap_new allocates and initializes a new hashmap and returns a pointer to
 * it. Both type interfaces must be given, and the type interface for keys must have a comparison
 * function and a hash function. */
int hashmap_initialize(hashmap *M, t_intf *kt, t_intf *vt)
{
    check_ptr(kt);
    check_ptr(vt);
    check(kt->compare, "no comparison function");
    check(kt->hash, "no hash function");

    M->key_type = kt;
    M->value_type = vt;
    M->count = 0;

    M->n_buckets = MAP_N_BUCKETS;
    M->buckets = calloc(M->n_buckets, sizeof(*M->buckets));
    check_alloc(M->buckets);

    return 0;
error:
    if (M->buckets) free(M->buckets);
    return -1;
}

hashmap *hashmap_new(t_intf *kt, t_intf *vt)
{
    hashmap *M = calloc(1, sizeof(*M));
    check_alloc(M);

    int rc = hashmap_initialize(M, kt, vt);
    check(rc == 0, "failed to initialize hashmap");

    return M;
error:
    if (M) free(M);
    return NULL;
}

/* void hashmap_clear(hashmap *M)
 * Delete all entries, releasing associated memory, and reset M. */
void hashmap_clear(hashmap *M)
{
    if (M && M->buckets) {
        hashmapn *cur;
        hashmapn *next;
        for (unsigned short i = 0; i < M->n_buckets; ++i) {
            cur = M->buckets[i];
            while (cur != NULL) {
                next = cur->next;
                hashmapn_delete(M, cur);
                cur = next;
            }
            M->buckets[i] = NULL;
        }
        M->count = 0;
    }
}

/* void hashmap_destroy(hashmap *M)
 * void hashmap_delete (hashmap *M)
 * Destroy M, freeing any associated memory. hashmap_delete also calls free on M. */
void hashmap_destroy(hashmap *M)
{
    if (M && M->buckets) {
        hashmap_clear(M);
        free(M->buckets);
        M->key_type = M->value_type = NULL;
        M->n_buckets = 0;
    }
}

void hashmap_delete(hashmap *M)
{
    if (M) {
        if (M->buckets) {
            hashmap_clear(M);
            free(M->buckets);
        }
        free(M);
    }
}

/* static inline hashmapn *hashmap_find_node(const hashmap *M, const void *k, unsigned short i)
 * Find the node with the key k in the bucket at index i. Return NULL if it's not there. */
static inline hashmapn *hashmap_find_node(const hashmap *M, const void *k, unsigned short i)
{
    assert(M && M->key_type && k && i < M->n_buckets);

    hashmapn *n = M->buckets[i];
    while (n) {
        if (t_compare(M->key_type, k, hashmapn_key(M, n)) == 0) return n;
        n = n->next;
    }
    return NULL;
}

/* int hashmap_set(hashmap *M, const void *k, const void *v)
 * Set the value of the node with the key k to v, or insert a node with k and v if k doesn't
 * exist. Return 1 if a node was added, 0 if k was already there, or -1 on error. */
int hashmap_set(hashmap *M, const void *k, const void *v)
{
    check_ptr(M);

    unsigned short i = t_hash(M->key_type, k) % MAP_N_BUCKETS;
    hashmapn *n = hashmap_find_node(M, k, i);

    if (n) {
        hashmapn_set_value(M, n, v);
        return 0;
    } else {
        n = hashmapn_new(M, k, v);
        check(n != NULL, "failed to create new node");
        n->next = M->buckets[i];
        M->buckets[i] = n;
        ++M->count;
        return 1;
    }

error:
    return -1;
}

/* int hashmap_has(const hashmap *M, const void *k)
 * Check if an entry with the key k exists. */
int hashmap_has(const hashmap *M, const void *k)
{
    check_ptr(M);
    check_ptr(k);

    unsigned short i = t_hash(M->key_type, k) % MAP_N_BUCKETS;
    hashmapn *n = hashmap_find_node(M, k, i);
    return n ? 1 : 0;

error:
    return -1;
}

/* void *hashmap_get(hashmap *M, const void *k)
 * Return a pointer to the value mapped to k in M or NULL if k doesn't exist. */
void *hashmap_get(hashmap *M, const void *k)
{
    check_ptr(M);
    check_ptr(k);

    unsigned short i = t_hash(M->key_type, k) % MAP_N_BUCKETS;
    hashmapn *n = hashmap_find_node(M, k, i);
    if (n)  return hashmapn_value(M, n);

error: /* fallthrough */
    return NULL;
}

/* int hashmap_remove(hashmap *M, const void *k)
 * Remove k from the map. Return 1 if a node was deleted, 0 if k was not there, or -1 on error. */
int hashmap_remove(hashmap *M, const void *k)
{
    check_ptr(M);
    check_ptr(k);

    unsigned short i = t_hash(M->key_type, k) % MAP_N_BUCKETS;
    hashmapn *node = M->buckets[i];
    hashmapn *prev = NULL;

    while (node && t_compare(M->key_type, k, hashmapn_key(M, node)) != 0) {
        prev = node;
        node = node->next;
    }

    if (node) {
        if (prev)   prev->next    = node->next;
        else        M->buckets[i] = node->next;
        hashmapn_delete(M, node);
        --M->count;
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}
