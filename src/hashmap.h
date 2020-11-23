#ifndef _map_h
#define _map_h

#include <stdlib.h>

#include "hash.h"
#include "sort_tools.h"
#include "type_interface.h"

#define MAP_N_BUCKETS 512

struct HashmapNode;
typedef struct HashmapNode {
    struct HashmapNode *next;
    char *key;
    char *value;
} HashmapNode;

typedef struct Hashmap {
    HashmapNode **buckets;
    size_t n_buckets;
    t_intf *key_type;
    t_intf *value_type;
} Hashmap;

int Hashmap_initialize(Hashmap *M, t_intf *key_type, t_intf *value_type);
Hashmap *Hashmap_new(t_intf *key_type, t_intf *value_type);
void Hashmap_delete(Hashmap *M);
void Hashmap_clear(Hashmap *M);

// These functions return 1 if found, 0 if not found, -1 on error.
int Hashmap_set(Hashmap *M, const void *key, const void *value);
int Hashmap_remove(Hashmap *M, const void *key);
int Hashmap_has(const Hashmap *M, const void *key);
int Hashmap_get(const Hashmap *M, const void *key, void *value_out);

#endif // _map_h
