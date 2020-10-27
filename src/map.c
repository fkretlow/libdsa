#include <assert.h>
#include <string.h>

#include "debug.h"
#include "map.h"

static inline _map_node *_map_node_new(void)
{
    _map_node *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void _map_node_delete(const Map M, _map_node *n)
{
    if (n) {
        if (n->key && M && M->destroy_key) {
            M->destroy_key(*(void**)n->key);
        }
        if (n->value && M && M->destroy_value) {
            M->destroy_value(*(void**)n->value);
        }
    }
    free(n->key);
    free(n->value);
    free(n);
}

static int _map_node_set_key(const Map M, _map_node *n, const void *key)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(key);

    /* Why would we ever overwrite an existing key? */
    assert(!n->key);

    n->key = malloc(M->key_size);
    check_alloc(n->key);

    if (M->copy_key) {
        M->copy_key(n->key, key);
    } else {
        memmove(n->key, key, M->key_size);
    }

    return 0;
error:
    return -1;
}

static int _map_node_set_value(const Map M, _map_node *n, const void *value)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(value);

    /* If we set a value, we must have a key. */
    assert(n->key);

    if (!n->value) {
        n->value = malloc(M->value_size);
        check_alloc(n->value);
    } else {
        if (M->destroy_value) {
            M->destroy_value(n->value);
        }
    }

    if (M->copy_value) {
        M->copy_value(n->value, value);
    } else {
        memmove(n->value, value, M->value_size);
    }

    return 0;
error:
    return -1;
}

static int _map_node_get_value(const Map M, _map_node *n, const void *out)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(out);

    assert(n->value);

    if (M->copy_value) {
        M->copy_value(out, n->value);
    } else {
        memmove(out, n->value, M->value_size);
    }

    return 0;
error:
    return -1;
}

Map Map_new(const size_t key_size, const size_t value_size,
            hash_f hash, compare_f compare,
            copy_f copy_key, destroy_f destroy_key,
            copy_f copy_value, destroy_f destroy_value)
{
    check_ptr(hash);
    check_ptr(compare);

    _map *M = malloc(sizeof(*M));

    M->key_size = key_size;
    M->value_size = value_size;
    M->hash = hash;
    M->compare = compare;
    M->copy_key = copy_key;
    M->destroy_key = destroy_key;
    M->copy_value = copy_value;
    M->destroy_value = destroy_value;

    M->n_buckets = MAP_N_BUCKETS;
    M->buckets = calloc(M->n_buckets, sizeof(*M->buckets));
    check_alloc(M->buckets);

    return M;
error:
    if (M) {
        if (M->buckets) free(M->buckets);
        free(M);
    }
    return NULL;
}

void Map_delete(Map M)
{
    if (M) {
        if (M->buckets) {
            Map_clear(M);
            free(M->buckets);
        }
        free(M);
    }
}

void Map_clear(Map M)
{
    if (M && M->buckets) {
        _map_node *cur;
        _map_node *next;
        for (size_t i = 0; i < M->n_buckets; ++i) {
            cur = M->buckets[i];
            while (cur != NULL) {
                next = cur->next;
                _map_node_delete(M, cur);
                cur = next;
            }
            M->buckets[i] = NULL;
        }
    }
}

_map_node *_map_find_node(const Map M, const void *key, size_t bucket_index)
{
    check_ptr(M);
    check_ptr(key);

    _map_node *n = M->buckets[bucket_index];
    if (!n) {
        return NULL;
    } else {
        while (n != NULL) {
            if (M->compare(n->key, key) == 0) {
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
int Map_set(Map M, const void *key, const void *value)
{
    check_ptr(M);

    // TODO: generic hash needs element size, but String_hash doesn't...
    size_t bucket_index = M->hash(key, M->key_size) % MAP_N_BUCKETS;
    _map_node *node = _map_find_node(M, key, bucket_index);

    if (node) {
        check(!_map_node_set_value(M, node, value), "Failed to set value.");
        return 1;
    } else {
        node = _map_node_new();
        check(node != NULL, "Failed to create new node.");
        check(!_map_node_set_key(M, node, key), "Failed to set key.");
        check(!_map_node_set_value(M, node, value), "Failed to set value.");
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
int Map_has(const Map M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = M->hash(key, M->key_size) % MAP_N_BUCKETS;
    _map_node *n = _map_find_node(M, key, bucket_index);

    return n ? 1 : 0;
error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Map_get(const Map M, const void *key, void *value_out)
{
    check_ptr(M);
    check_ptr(key);
    check_ptr(value_out);

    size_t bucket_index = M->hash(key, M->key_size) % MAP_N_BUCKETS;
    _map_node *n = _map_find_node(M, key, bucket_index);

    if (n) {
        if (M->copy_value) {
            M->copy_value(value_out, n->value);
        } else {
            memmove(value_out, n->value, M->value_size);
        }
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
int Map_remove(Map M, const void *key)
{
    check_ptr(M);
    check_ptr(key);

    size_t bucket_index = M->hash(key, M->key_size) % MAP_N_BUCKETS;

    _map_node *node = M->buckets[bucket_index];
    _map_node *prev = NULL;

    while (node && M->compare(node->key, key) != 0) {
        prev = node;
        node = node->next;
    }

    if (node) {
        if (prev) {
            prev->next = node->next;
        } else {
            M->buckets[bucket_index] = node->next;
        }
        _map_node_delete(M, node);
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}
