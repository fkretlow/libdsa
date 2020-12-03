/*************************************************************************************************
 *
 * hashmap.h
 *
 * Interface for a hashmap that supports arbitrary key/value types by way of type interfaces.
 *
 * Author: Florian Kretlow, 2020
 * Use, modify, and distribute as you wish.
 *
 ************************************************************************************************/

#ifndef _hashmap_h
#define _hashmap_h

#include "hash.h"
#include "type_interface.h"

#define MAP_N_BUCKETS 512

struct hashmapn;
typedef struct hashmapn {
    struct hashmapn *   next;
} hashmapn;

typedef struct hashmap {
    hashmapn **     buckets;
    unsigned short  n_buckets;
    size_t          count;
    t_intf *        key_type;
    t_intf *        value_type;
} hashmap;

#define hashmap_count(M) ((M)->count)

int         hashmap_initialize (hashmap *M, t_intf *kt, t_intf *vt);
hashmap *   hashmap_new        (            t_intf *kt, t_intf *vt);
void        hashmap_destroy    (hashmap *M);
void        hashmap_delete     (hashmap *M);

void        hashmap_clear      (hashmap *M);

int         hashmap_set        (      hashmap *M, const void *k, const void *v);
int         hashmap_remove     (      hashmap *M, const void *k);
int         hashmap_has        (const hashmap *M, const void *k);
void *      hashmap_get        (      hashmap *M, const void *k);

#endif // _hashmap_h
