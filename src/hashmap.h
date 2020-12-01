#ifndef _map_h
#define _map_h

#include <stdlib.h>

#include "hash.h"
#include "sort_tools.h"
#include "type_interface.h"

#define MAP_N_BUCKETS 512

struct hashmapn;
typedef struct hashmapn {
    struct hashmapn *   next;
    char *              key;
    char *              value;
} hashmapn;

typedef struct hashmap {
    hashmapn **     buckets;
    size_t          n_buckets;
    t_intf *        key_type;
    t_intf *        value_type;
} hashmap;

int         hashmap_initialize (hashmap *M, t_intf *kt, t_intf *vt);
hashmap *   hashmap_new        (            t_intf *kt, t_intf *vt);
void        hashmap_destroy    (hashmap *M);
void        hashmap_delete     (hashmap *M);

void        hashmap_clear      (hashmap *M);

int         hashmap_set        (      hashmap *M, const void *k, const void *v);
int         hashmap_remove     (      hashmap *M, const void *k);
int         hashmap_has        (const hashmap *M, const void *k);
void *      hashmap_get        (      hashmap *M, const void *k);

#endif // _map_h
