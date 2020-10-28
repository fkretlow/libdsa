#ifndef _map_h
#define _map_h

#include <stdlib.h>

#include "container_tools.h"
#include "hash.h"
#include "sort_tools.h"
#include "type_interface.h"

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
    TypeInterface *key_type;
    TypeInterface *value_type;
} _map;

typedef _map *Map;

Map Map_new(TypeInterface *key_type, TypeInterface *value_type);
void Map_delete(Map M);
void Map_clear(Map M);

// These functions return 1 if found, 0 if not found, -1 on error.
int Map_set(Map M, const void *key, const void *value);
int Map_remove(Map M, const void *key);
int Map_has(const Map M, const void *key);
int Map_get(const Map M, const void *key, void *value_out);

#endif // _map_h
