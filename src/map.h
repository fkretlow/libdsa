#ifndef _map_h
#define _map_h

#include <stdlib.h>

#include "container_tools.h"
#include "sort_tools.h"

#define MAP_N_BUCKETS 512

struct __MapNode;
typedef struct __MapNode {
    struct __MapNode* next;
    char* key;
    char* value;
} __MapNode;

typedef struct Map {
    size_t key_size;
    size_t value_size;
    __MapNode** buckets;
    size_t n_buckets;
    __hash_f hash;
    __compare_f compare;
    __destroy_f destroy_key;
    __destroy_f destroy_value;
} Map;

int Map_init(Map* m, const size_t key_size, const size_t value_size,
             __hash_f hash, __compare_f compare,
             __destroy_f destroy_key, __destroy_f destroy_value);
void Map_clear(Map* m);
void Map_destroy(Map* m);

int Map_set(Map* m, const void* key, const void* value);
int Map_has(const Map* m, const void* key, int* result_out);
int Map_get(const Map* m, const void* key, void* value_out, const void* deflt);
int Map_delete(Map* m, const void* key);

#endif // _map_h
