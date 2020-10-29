#include <assert.h>
#include <string.h>

#include "debug.h"
#include "map.h"

extern void *TypeInterface_allocate(TypeInterface *T, size_t n);
extern void TypeInterface_copy(TypeInterface *T, void *dest, const void *src);
extern void TypeInterface_destroy(TypeInterface *T, void *obj);
extern int TypeInterface_compare(TypeInterface *T, const void *a, const void *b);
extern unsigned long TypeInterface_hash(TypeInterface *T, const void *obj);

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
        if (n->key && M) TypeInterface_destroy(M->key_type, n->key);
        free(n->key);
        if (n->value && M) TypeInterface_destroy(M->value_type, n->value);
        free(n->value);
        free(n);
    }
}

static int _map_node_set_key(const Map M, _map_node *n, const void *key)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(key);

    /* Why would we ever overwrite an existing key? */
    assert(!n->key);

    n->key = TypeInterface_allocate(M->key_type, 1);
    check(n->key != NULL, "Failed to allocate memory for key.");
    TypeInterface_copy(M->key_type, (void*)n->key, key);

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
        n->value = TypeInterface_allocate(M->value_type, 1);
        check(n->value != NULL, "Failed to allocate memory for value.");
    } else {
        TypeInterface_destroy(M->value_type, n->value);
    }

    TypeInterface_copy(M->value_type, (void*)n->value, value);

    return 0;
error:
    return -1;
}

static int _map_node_get_value(const Map M, _map_node *n, void *out)
{
    check_ptr(M);
    check_ptr(n);
    check_ptr(out);

    assert(n->value);

    TypeInterface_copy(M->value_type, out, (void*)n->value);

    return 0;
error:
    return -1;
}

Map Map_new(TypeInterface *key_type, TypeInterface *value_type)
{
    check_ptr(key_type);
    check_ptr(value_type);
    check(key_type->compare != NULL, "No comparison function provided for key type.");

    _map *M = malloc(sizeof(*M));
    check_alloc(M);

    M->key_type = key_type;
    M->value_type = value_type;

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
            if (TypeInterface_compare(M->key_type, n->key, key) == 0) {
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

    size_t bucket_index = TypeInterface_hash(M->key_type, key) % MAP_N_BUCKETS;
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

    size_t bucket_index = TypeInterface_hash(M->key_type, key) % MAP_N_BUCKETS;
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

    size_t bucket_index = TypeInterface_hash(M->key_type, key) % MAP_N_BUCKETS;
    _map_node *n = _map_find_node(M, key, bucket_index);

    if (n) {
        check(!_map_node_get_value(M, n, value_out), "Failed to hand out value.");
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

    size_t bucket_index = TypeInterface_hash(M->key_type, key) % MAP_N_BUCKETS;

    _map_node *node = M->buckets[bucket_index];
    _map_node *prev = NULL;

    while (node && TypeInterface_compare(M->key_type, node->key, key) != 0) {
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
