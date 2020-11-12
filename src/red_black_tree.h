#ifndef _rbt_h
#define _rbt_h

#include <stdlib.h>
#include <string.h>

#include "sort_tools.h"
#include "type_interface.h"

#define BLACK 0
#define RED 1

#define RBT_ALLOC_THRESHOLD (2 * sizeof(char*))

union RBTreeData {
    struct {
        char data[RBT_ALLOC_THRESHOLD];
    } internal;
    struct {
        char *key;
        char *value;
    } external;
};

struct RBTreeNode;
typedef struct RBTreeNode {
    struct RBTreeNode *parent;
    struct RBTreeNode *left;
    struct RBTreeNode *right;
    unsigned int color     : 1;
    unsigned int has_key   : 1;
    unsigned int has_value : 1;
    union RBTreeData data;
} RBTreeNode;

typedef struct RBTree {
    RBTreeNode *root;
    size_t size;
    TypeInterface *key_type;
    TypeInterface *value_type;
    unsigned int storage_allocated : 1;
} RBTree;


int RBTree_initialize(RBTree *T, TypeInterface *key_type);
void RBTree_clear(RBTree *T);
int RBTree_copy(RBTree *dest, const RBTree *src);

// These functions return 1 if found, 0 if not found, -1 on error.
int RBTree_insert(RBTree *T, const void *value);
int RBTree_remove(RBTree *T, const void *value);
int RBTree_has(const RBTree *T, const void *value);

int RBTree_traverse(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);
int RBTree_traverse_r(RBTree *T, int (*f)(RBTreeNode *n, void *p), void *p);

RBTreeNode *RBTreeNode_new(void);
void RBTreeNode_delete(const RBTree *T, RBTreeNode *n);
int RBTreeNode_set(const RBTree *T, RBTreeNode *n, const void *value);
int RBTreeNode_rotate_left(RBTree *T, RBTreeNode *n, RBTreeNode **node_out);
int RBTreeNode_rotate_right(RBTree *T, RBTreeNode *n, RBTreeNode **node_out);

#define RBTreeNode_key(T, n) ( (T)->storage_allocated \
        ? (n)->data.external.key \
        : (n)->data.internal.data )
#define RBTreeNode_value(T, n) ( (T)->storage_allocated \
        ? (n)->data.external.value \
        : (n)->data.internal.data + TypeInterface_size((T)->key_type) )

#endif // _rbt_h
