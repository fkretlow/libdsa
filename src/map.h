#ifndef _map_h
#define _map_h

#include <stdlib.h>

#include "container_tools.h"
#include "hash.h"
#include "sort_tools.h"

#define MAP_N_BUCKETS 512

struct _map_node;
typedef struct _map_node {
    struct _map_node *next;
    char *key;
    char *value;
} _map_node;

typedef struct _map {
    _map_node **buckets;
    size_t n_buckets;
    size_t key_size;
    size_t value_size;
    hash_f hash;
    compare_f compare;
    copy_f copy_key;
    destroy_f destroy_key;
    copy_f copy_value;
    destroy_f destroy_value;
} _map;

typedef _map *Map;

Map Map_new(const size_t key_size, const size_t value_size,
            hash_f hash, compare_f compare,
            copy_f copy_key, destroy_f destroy_key,
            copy_f copy_value, destroy_f destroy_value);
void Map_delete(Map M);
void Map_clear(Map M);

// These functions return 1 if found, 0 if not found, -1 on error.
int Map_set(Map M, const void *key, const void *value);
int Map_remove(Map M, const void *key);
int Map_has(const Map M, const void *key);
int Map_get(const Map M, const void *key, void *value_out);

#endif // _map_h
