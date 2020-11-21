#ifndef _map_h
#define _map_h

#include "binary_tree.h"

typedef bt map;

#define map_initialize(M, kt, vt)       bt_initialize(M, RED_BLACK, kt, vt)
#define map_new(kt, vt)                 bt_new(RED_BLACK, kt, vt)
#define map_destroy(M)                  bt_destroy(M)
#define map_delete(M)                   bt_delete(M)

#define map_clear(M)                    bt_clear(M)
#define map_copy(M)                     bt_copy(M)
#define map_copy_to(dest, src)          bt_copy_to(dest, src)

#define map_set(M, k, v)                bt_set(M, k, v)
#define map_get(M, k)                   bt_get(M, k)
#define map_has(M, k)                   bt_has(M, k)

#define map_count(M)                    bt_count(M)

#endif /* _map_h */
