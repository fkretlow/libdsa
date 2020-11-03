#ifndef _rbt_h
#define _rbt_h

#include <stdlib.h>
#include <string.h>

#include "sort_tools.h"
#include "type_interface.h"

#define BLACK 0
#define RED 1

struct _rbt_node;
typedef struct _rbt_node {
    struct _rbt_node *parent;
    struct _rbt_node *left;
    struct _rbt_node *right;
    char *data;
    unsigned int color : 1;
} _rbt_node;

typedef struct _rbt {
    _rbt_node *root;
    size_t size;
    TypeInterface *element_type;
} _rbt;


int _rbt_init(_rbt *T, TypeInterface *element_type);
void _rbt_clear(_rbt *T);

// These functions return 1 if found, 0 if not found, -1 on error.
int _rbt_insert(_rbt *T, const void *value);
int _rbt_remove(_rbt *T, const void *value);
int _rbt_has(const _rbt *T, const void *value);

int _rbt_traverse(const _rbt *T, int (*f)(_rbt_node *n, void *p), void *p);

int _rbt_node_new(_rbt_node **node_out);
void _rbt_node_delete(const _rbt *T, _rbt_node *n);
int _rbt_node_set(const _rbt *T, _rbt_node *n, const void *value);
int _rbt_node_rotate_left(_rbt *T, _rbt_node *n, _rbt_node **node_out);
int _rbt_node_rotate_right(_rbt *T, _rbt_node *n, _rbt_node **node_out);

#endif // _rbt_h
