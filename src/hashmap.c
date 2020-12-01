#include <assert.h>
#include <string.h>

#include "check.h"
#include "hashmap.h"

static inline hashmapn *hashmapn_new(void)
{
    hashmapn *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void hashmapn_delete(const hashmap *M, hashmapn *n)
{
    if (n) {
        if (n->key && M) t_destroy(M->key_type, n->key);
        free(n->key);
        if (n->value && M) t_destroy(M->value_type, n->value);
        free(n->value);
        free(n);
    }
}

static int hashmapn_set_key(const hashmap *M, hashmapn *n, const void *key)
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

static int hashmapn_set_value(const hashmap *M, hashmapn *n, const void *value)
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

static int hashmapn_get_value(const hashmap *M, hashmapn *n, void *out)
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

int hashmap_initialize(hashmap *M, t_intf *kt, t_intf *vt)
{
    check_ptr(kt);
    check_ptr(vt);
    check(kt->compare != NULL, "No comparison function provided for key type.");

    M->key_type = kt;
    M->value_type = vt;

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
    check(rc == 0, "Failed to initialize hashmap.");

    return M;
error:
    if (M) free(M);
    return NULL;
}

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

void hashmap_clear(hashmap *M)
{
    if (M && M->buckets) {
        hashmapn *cur;
        hashmapn *next;
        for (size_t i = 0; i < M->n_buckets; ++i) {
            cur = M->buckets[i];
            while (cur != NULL) {
                next = cur->next;
                hashmapn_delete(M, cur);
                cur = next;
            }
            M->buckets[i] = NULL;
        }
    }
}

hashmapn *hashmap_find_node(const hashmap *M, const void *key, size_t bucket_index)
{
    check_ptr(M);
    check_ptr(key);

    hashmapn *n = M->buckets[bucket_index];
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
int hashmap_set(hashmap *M, const void *key, const void *value)
{
    check_ptr(M);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    hashmapn *node = hashmap_find_node(M, key, bucket_index);

    if (node) {
        check(!hashmapn_set_value(M, node, value), "Failed to set value.");
        return 1;
    } else {
        node = hashmapn_new();
        check(node != NULL, "Failed to create new node.");
        check(!hashmapn_set_key(M, node, key), "Failed to set key.");
        check(!hashmapn_set_value(M, node, value), "Failed to set value.");
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
int hashmap_has(const hashmap *M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    hashmapn *n = hashmap_find_node(M, key, bucket_index);

    return n ? 1 : 0;
error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int hashmap_get(const hashmap *M, const void *key, void *value_out)
{
    check_ptr(M);
    check_ptr(key);
    check_ptr(value_out);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;
    hashmapn *n = hashmap_find_node(M, key, bucket_index);

    if (n) {
        check(!hashmapn_get_value(M, n, value_out), "Failed to hand out value.");
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
int hashmap_remove(hashmap *M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = t_hash(M->key_type, key) % MAP_N_BUCKETS;

    hashmapn *node = M->buckets[bucket_index];
    hashmapn *prev = NULL;

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
        hashmapn_delete(M, node);
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}
