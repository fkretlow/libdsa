#include <assert.h>
#include <string.h>

#include "debug.h"
#include "hashmap.h"

static inline HashmapNode *HashmapNode_new(void)
{
    HashmapNode *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void HashmapNode_delete(const Hashmap *M, HashmapNode *n)
{
    if (n) {
        if (n->key && M) t_destroy(M->key_type, n->key);
        free(n->key);
        if (n->value && M) t_destroy(M->value_type, n->value);
        free(n->value);
        free(n);
    }
}

static int HashmapNode_set_key(const Hashmap *M, HashmapNode *n, const void *key)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(key);

    /* Why would we ever overwrite an existing key? */
    assert(!n->key);

    n->key = t_allocate(M->key_type, 1);
    check(n->key != NULL, "Failed to allocate memory for key.");
    t_copy(M->key_type, (void*)n->key, key);

    return 0;
error:
    return -1;
}

static int HashmapNode_set_value(const Hashmap *M, HashmapNode *n, const void *value)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(value);

    /* If we set a value, we must have a key. */
    assert(n->key);

    if (!n->value) {
        n->value = t_allocate(M->value_type, 1);
        check(n->value != NULL, "Failed to allocate memory for value.");
    } else {
        t_destroy(M->value_type, n->value);
    }

    t_copy(M->value_type, (void*)n->value, value);

    return 0;
error:
    return -1;
}

static int HashmapNode_get_value(const Hashmap *M, HashmapNode *n, void *out)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(out);

    assert(n->value);

    t_copy(M->value_type, out, (void*)n->value);

    return 0;
error:
    return -1;
}

int Hashmap_initialize(Hashmap *M, t_intf *key_type, t_intf *value_type)
{
    check_ptr(key_type);
    check_ptr(value_type);
    check(key_type->compare != NULL, "No comparison function provided for key type.");

    M->key_type = key_type;
    M->value_type = value_type;

    M->n_buckets = MAP_N_BUCKETS;
    M->buckets = calloc(M->n_buckets, sizeof(*M->buckets));
    check_alloc(M->buckets);

    return 0;
error:
    if (M->buckets) free(M->buckets);
    return -1;
}

Hashmap *Hashmap_new(t_intf *key_type, t_intf *value_type)
{
    Hashmap *M = calloc(1, sizeof(*M));
    check_alloc(M);

    int rc = Hashmap_initialize(M, key_type, value_type);
    check(rc == 0, "Failed to initialize Hashmap.");

    return M;
error:
    if (M) free(M);
    return NULL;
}

void Hashmap_delete(Hashmap *M)
{
    if (M) {
        if (M->buckets) {
            Hashmap_clear(M);
            free(M->buckets);
        }
        free(M);
    }
}

void Hashmap_clear(Hashmap *M)
{
    if (M && M->buckets) {
        HashmapNode *cur;
        HashmapNode *next;
        for (size_t i = 0; i < M->n_buckets; ++i) {
            cur = M->buckets[i];
            while (cur != NULL) {
                next = cur->next;
                HashmapNode_delete(M, cur);
                cur = next;
            }
            M->buckets[i] = NULL;
        }
    }
}

HashmapNode *Hashmap_find_node(const Hashmap *M, const void *key, size_t bucket_index)
{
    check_ptr(M);
    check_ptr(key);

    HashmapNode *n = M->buckets[bucket_index];
    if (!n) {
        return NULL;
    } else {
        while (n != NULL) {
            if (t_compare(M->key_type, n->key, key) == 0) {
                return n;
            }
            n = n->next;
        }
    }

error:
    return NULL;
}

// Return values: 1 if the key already exists
//                0 if a new entry was created
//               -1 on error
int Hashmap_set(Hashmap *M, const void *key, const void *value)
{
    check_ptr(M);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    HashmapNode *node = Hashmap_find_node(M, key, bucket_index);

    if (node) {
        check(!HashmapNode_set_value(M, node, value), "Failed to set value.");
        return 1;
    } else {
        node = HashmapNode_new();
        check(node != NULL, "Failed to create new node.");
        check(!HashmapNode_set_key(M, node, key), "Failed to set key.");
        check(!HashmapNode_set_value(M, node, value), "Failed to set value.");
        node->next = M->buckets[bucket_index];
        M->buckets[bucket_index] = node;
        return 0;
    }

error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Hashmap_has(const Hashmap *M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    HashmapNode *n = Hashmap_find_node(M, key, bucket_index);

    return n ? 1 : 0;
error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Hashmap_get(const Hashmap *M, const void *key, void *value_out)
{
    check_ptr(M);
    check_ptr(key);
    check_ptr(value_out);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    HashmapNode *n = Hashmap_find_node(M, key, bucket_index);

    if (n) {
        check(!HashmapNode_get_value(M, n, value_out), "Failed to hand out value.");
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Hashmap_remove(Hashmap *M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;

    HashmapNode *node = M->buckets[bucket_index];
    HashmapNode *prev = NULL;

    while (node && t_compare(M->key_type, node->key, key) != 0) {
        prev = node;
        node = node->next;
    }

    if (node) {
        if (prev) {
            prev->next = node->next;
        } else {
            M->buckets[bucket_index] = node->next;
        }
        HashmapNode_delete(M, node);
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}
