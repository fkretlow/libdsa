#include <string.h>

#include "debug.h"
#include "map.h"

static inline int MapNode_new(MapNode **node_out)
{
    MapNode *new = calloc(1, sizeof(*new));
    check_alloc(new);
    *node_out = new;
    return 0;
error:
    return -1;
}

static inline void MapNode_delete(const Map *m, MapNode *n)
{
    if (n) {
        if (n->key && m && m->destroy_key) {
            m->destroy_key(n->key);
        }
        if (n->value && m && m->destroy_value) {
            m->destroy_value(n->value);
        }
    }
    free(n->key);
    free(n->value);
    free(n);
}

static inline int MapNode_set_key(const Map *m, MapNode *n, const void *key)
{
    check_ptr(m);
    check_ptr(n);
    check_ptr(key);

    if (!n->key) {
        n->key = malloc(m->key_size);
        check_alloc(n->key);
    }

    memmove(n->key, key, m->key_size);

    return 0;
error:
    return -1;
}

static inline int MapNode_set_value(const Map *m, MapNode *n, const void *value)
{
    check_ptr(m);
    check_ptr(n);
    check_ptr(value);

    if (!n->value) {
        n->value = malloc(m->value_size);
        check_alloc(n->value);
    } else {
        if (m->destroy_value) {
            m->destroy_value(n->value);
        }
    }

    memmove(n->value, value, m->value_size);

    return 0;
error:
    return -1;
}

int Map_init(Map *m, const size_t key_size, const size_t value_size,
             _hash_f hash, _compare_f compare,
             _destroy_f destroy_key, _destroy_f destroy_value)
{
    check_ptr(m);
    check_ptr(hash);
    check_ptr(compare);

    m->key_size = key_size;
    m->value_size = value_size;
    m->hash = hash;
    m->compare = compare;
    m->destroy_key = destroy_key;
    m->destroy_value = destroy_value;

    m->n_buckets = MAP_N_BUCKETS;
    m->buckets = calloc(m->n_buckets, sizeof(*m->buckets));
    check_alloc(m->buckets);

    return 0;
error:
    if (m->buckets) free(m->buckets);
    return -1;
}

void Map_clear(Map *m)
{
    MapNode *cur;
    MapNode *next;
    for (size_t i = 0; i < m->n_buckets; ++i) {
        cur = m->buckets[i];
        while (cur) {
            next = cur->next;
            MapNode_delete(m, cur);
            cur = next;
        }
        m->buckets[i] = NULL;
    }
}

void Map_destroy(Map *m)
{
    Map_clear(m);
    free(m->buckets);
}

int __Map_find_node(const Map *m, const void *key, size_t bucket_index,
                    MapNode **node_out)
{
    check_ptr(m);
    check_ptr(key);
    check_ptr(node_out);

    MapNode *n = m->buckets[bucket_index];
    if (!n) {
        *node_out = NULL;
    } else {
        while (n != NULL) {
            if (m->compare(n->key, key) == 0) {
                *node_out = n;
                break;
            }
        }
    }

    return 0;
error:
    return -1;
}

// Return values: 1 if the key already exists
//                0 if a new entry was created
//               -1 on error
int Map_set(Map *m, const void *key, const void *value)
{
    check_ptr(m);

    size_t bucket_index = m->hash(key, m->key_size) % MAP_N_BUCKETS;
    MapNode *node = NULL;

    check(!__Map_find_node(m, key, bucket_index, &node), "Failed to find node.");

    if (node) {
        check(!MapNode_set_value(m, node, value), "Failed to set value.");
        return 1;
    } else {
        check(!MapNode_new(&node), "Failed to create new node.");
        check(!MapNode_set_key(m, node, key), "Failed to set key.");
        check(!MapNode_set_value(m, node, value), "Failed to set value.");
        node->next = m->buckets[bucket_index];
        m->buckets[bucket_index] = node;
        return 0;
    }

error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Map_has(const Map *m, const void *key)
{
    check_ptr(m);
    check_ptr(key);

    size_t bucket_index = m->hash(key, m->key_size) % MAP_N_BUCKETS;
    MapNode *n = NULL;
    check(!__Map_find_node(m, key, bucket_index, &n), "Failed to find node.");

    return n ? 1 : 0;
error:
    return -1;
}

// Return values: 1 if found
//                0 if not found
//               -1 on error
int Map_get(const Map *m, const void *key, void *value_out)
{
    check_ptr(m);
    check_ptr(key);
    check_ptr(value_out);

    size_t bucket_index = m->hash(key, m->key_size) % MAP_N_BUCKETS;
    MapNode *n = NULL;
    check(!__Map_find_node(m, key, bucket_index, &n), "Failed to find node.");

    if (n) {
        memmove(value_out, n->value, m->value_size);
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
int Map_delete(Map *m, const void *key)
{
    check_ptr(m);
    check_ptr(key);

    size_t bucket_index = m->hash(key, m->key_size) % MAP_N_BUCKETS;

    MapNode *node = m->buckets[bucket_index];
    MapNode *prev = NULL;

    while (node && m->compare(node->key, key) != 0) {
        prev = node;
        node = node->next;
    }

    if (node) {
        if (prev) {
            prev->next = node->next;
        } else {
            m->buckets[bucket_index] = node->next;
        }
        MapNode_delete(m, node);
        return 1;
    } else {
        return 0;
    }

error:
    return -1;
}
