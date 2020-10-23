#ifndef _rbt_h
#define _rbt_h

#include <stdlib.h>
#include <string.h>

#include "container_tools.h"
#include "sort_tools.h"

#define BLACK 0
#define RED 1

struct __rbt_node;
typedef struct __rbt_node {
    struct __rbt_node *parent;
    struct __rbt_node *left;
    struct __rbt_node *right;
    char color;
    char *data;
} __rbt_node;

typedef struct __rbt {
    __rbt_node *root;
    size_t element_size;
    size_t size;
    __compare_f compare;
    __destroy_f destroy;
} __rbt;

int __rbt_init(__rbt *T,
               const size_t element_size,
               __compare_f compare,
               __destroy_f destroy);
void __rbt_clear(__rbt *T);

// These functions return 1 if found, 0 if not found, -1 on error.
int __rbt_insert(__rbt *T, const void *value);
int __rbt_delete(__rbt *T, const void *value);
int __rbt_has(__rbt *T, const void *value);

int __rbt_traverse(__rbt *T, __traverse_f f);

int __rbt_node_new(__rbt_node **node_out);
void __rbt_node_delete(const __rbt *T, __rbt_node *n);
int __rbt_node_set(const __rbt *T, __rbt_node *n, const void *value);
int __rbt_node_rotate_left(__rbt *T, __rbt_node *n, __rbt_node **node_out);
int __rbt_node_rotate_right(__rbt *T, __rbt_node *n, __rbt_node **node_out);

#endif // _rbt_h
