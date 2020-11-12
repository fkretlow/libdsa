#ifndef _binary_tree_h
#define _binary_tree_h

#include <stddef.h>

#include "data.h"

struct BinaryTreeNode;
typedef struct BinaryTreeNode {
    struct BinaryTreeNode *parent;
    struct BinaryTreeNode *left;
    struct BinaryTreeNode *right;
    unsigned int flags : 8;
    union MappingData data;
} BinaryTreeNode;

#define BinaryTreeNode_key(T, n) \
    ( (T)->storage_allocated ? (n)->data.external.key \
                             : (n)->data.internal.data )
#define BinaryTreeNode_value(T, n) \
    ( (T)->storage_allocated ? (n)->data.external.value \
                             : (n)->data.internal.data + TypeInterface_size((T)->key_size) )

typedef struct BinaryTree {
    BinaryTreeNode *root;
    size_t size;
    TypeInterface *key_type;
    TypeInterface *value_type;
    unsigned int storage_allocated : 1;
    unsigned int flags             : 7;
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
