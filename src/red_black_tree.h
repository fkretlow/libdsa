#ifndef _red_black_tree_h
#define _red_black_tree_h

#include <stdlib.h>
#include <string.h>

#include "node_data.h"
#include "sort_tools.h"
#include "type_interface.h"

#define BLACK 0
#define RED 1

#define RBT_ALLOC_THRESHOLD (2 * sizeof(char*))

struct RBTreeNode;
typedef struct RBTreeNode {
    struct RBTreeNode *parent;
    struct RBTreeNode *left;
    struct RBTreeNode *right;
    unsigned int color     : 1;
    unsigned int has_key   : 1;
    unsigned int has_value : 1;
    union MappingData data;
} RBTreeNode;

typedef struct RBTree {
    RBTreeNode *root;
    size_t size;
    TypeInterface *key_type;
    TypeInterface *value_type;
    unsigned int external_storage : 1;
} RBTree;


int RBTree_initialize(RBTree *T, TypeInterface *key_type, TypeInterface *value_type);
void RBTree_clear(RBTree *T);
int RBTree_copy(RBTree *dest, const RBTree *src);

// These functions return 1 if found, 0 if not found, -1 on error.
int RBTree_insert(RBTree *T, const void *k);
int RBTree_remove(RBTree *T, const void *k);
int RBTree_has(const RBTree *T, const void *k);

int RBTree_traverse_nodes(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);
int RBTree_traverse_nodes_r(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);
int RBTree_traverse_keys(RBTree *T, int (*f)(void *k, void *p), void *p);
int RBTree_traverse_keys_r(RBTree *T, int (*f)(void *k, void *p), void *p);
int RBTree_traverse_values(RBTree *T, int (*f)(void *v, void *p), void *p);
int RBTree_traverse_values_r(RBTree *T, int (*f)(void *v, void *p), void *p);

int RBTree_invariant(const RBTree *T);

RBTreeNode *RBTreeNode_new(void);
void RBTreeNode_delete(RBTree *T, RBTreeNode *n);
int RBTreeNode_set_key(const RBTree *T, RBTreeNode *n, const void *k);
int RBTreeNode_set_value(const RBTree *T, RBTreeNode *n, const void *v);
int RBTreeNode_rotate_left(RBTree *T, RBTreeNode *n, RBTreeNode **node_out);
int RBTreeNode_rotate_right(RBTree *T, RBTreeNode *n, RBTreeNode **node_out);

#define RBTreeNode_key(T, n) ( (T)->external_storage \
        ? (n)->data.external.key \
        : (n)->data.internal.data )
#define RBTreeNode_value(T, n) ( (T)->external_storage \
        ? (n)->data.external.value \
        : (n)->data.internal.data + TypeInterface_size((T)->key_type) )

#endif // _red_black_tree_h
