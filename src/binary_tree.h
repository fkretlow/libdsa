#ifndef _binary_tree_h
#define _binary_tree_h

#include <stddef.h>

#include "node_data.h"
#include "type_interface.h"

struct BinaryTreeNodeFlags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct BinaryTreeNode;
typedef struct BinaryTreeNode {
    struct BinaryTreeNode *parent;
    struct BinaryTreeNode *left;
    struct BinaryTreeNode *right;
    struct BinaryTreeNodeFlags flags;
    MappingData data;
} BinaryTreeNode;

#define BinaryTreeNode_key(T, n) \
    ( (T)->storage_allocated ? (n)->data.external.key \
                             : (n)->data.internal.data )
#define BinaryTreeNode_value(T, n) \
    ( (T)->storage_allocated ? (n)->data.external.value \
                             : (n)->data.internal.data + TypeInterface_size((T)->key_size) )

struct BinaryTreeFlags {
    unsigned char memory_scheme : 2;
};

typedef struct {
    BinaryTreeNode *root;
    size_t size;
    TypeInterface *key_type;
    TypeInterface *value_type;
    struct BinaryTreeFlags flags;
} BinaryTree;

int BinaryTree_initialize(BinaryTree *T, TypeInterface *key_type, TypeInterface *value_type);
void BinaryTree_destroy(BinaryTree *T);
BinaryTree *BinaryTree_new(TypeInterface *key_type, TypeInterface *value_type);
void BinaryTree_delete(BinaryTree *T);

BinaryTree *BinaryTree_copy(BinaryTree *dest, BinaryTree *src);

int BinaryTree_insert(BinaryTree *T, const void *k);
int BinaryTree_remove(BinaryTree *T, const void *k);
int BinaryTree_set(BinaryTree *T, const void *k, const void *v);
int BinaryTree_get(BinaryTree *T, const void *k, void *v_out);
void BinaryTree_clear(BinaryTree *T);

int BinaryTree_traverse_keys (BinaryTree *T, int (*f)(void *k, void *p), void *p);
int BinaryTree_traverse_keys_r (BinaryTree *T, int (*f)(void *k, void *p), void *p);
int BinaryTree_traverse_values (BinaryTree *T, int (*f)(void *v, void *p), void *p);
int BinaryTree_traverse_values_r(BinaryTree *T, int (*f)(void *v, void *p), void *p);
int BinaryTree_traverse_nodes (BinaryTree *T, int (*f)(BinaryTreeNode *n, void *p), void *p);
int BinaryTree_traverse_nodes_r (BinaryTree *T, int (*f)(BinaryTreeNode *n, void *p), void *p);

#define BinaryTree_size(T) (T)->size

#endif // _binary_tree_h
