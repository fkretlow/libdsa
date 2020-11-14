#ifndef _binary_tree_h
#define _binary_tree_h

#include <stddef.h>

#include "node_data.h"
#include "type_interface.h"

struct BinaryTreeNodeFlags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct RBTreeNodeFlags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    unsigned char color     : 1;
};

struct AVLTreeNodeFlags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    char balance            : 3;
};

struct BinaryTreeNode;
typedef struct BinaryTreeNode {
    struct BinaryTreeNode *parent;
    struct BinaryTreeNode *left;
    struct BinaryTreeNode *right;
    union {
        struct BinaryTreeNodeFlags plain;
        struct RBTreeNodeFlags red_black;
        struct AVLTreeNodeFlags avl;
    } flags;
    char data[MAPPING_DATA_SIZE];
} BinaryTreeNode;

#define NONE 0
#define RED_BLACK_TREE 1
#define AVL_TREE 2

typedef struct {
    BinaryTreeNode *root;
    size_t count;
    MemoryScheme memory_scheme;
    int balancing_strategy;
} BinaryTree;

int BinaryTree_initialize(BinaryTree *T, int balancing_strategy,
                          TypeInterface *key_type, TypeInterface *value_type);
BinaryTree *BinaryTree_new(int balancing_strategy,
                           TypeInterface *key_type, TypeInterface *value_type);
void BinaryTree_destroy(BinaryTree *T);
void BinaryTree_delete(BinaryTree *T);

BinaryTree *BinaryTree_copy(BinaryTree *dest, BinaryTree *src);

int BinaryTree_insert(BinaryTree *T, const void *k);
int BinaryTree_remove(BinaryTree *T, const void *k);
int BinaryTree_set(BinaryTree *T, const void *k, const void *v);
int BinaryTree_get(BinaryTree *T, const void *k, void *v_out);
void BinaryTree_clear(BinaryTree *T);

int BinaryTree_traverse_keys(BinaryTree *T, int (*f)(void *k, void *p), void *p);
int BinaryTree_traverse_keys_r(BinaryTree *T, int (*f)(void *k, void *p), void *p);
int BinaryTree_traverse_values(BinaryTree *T, int (*f)(void *v, void *p), void *p);
int BinaryTree_traverse_values_r(BinaryTree *T, int (*f)(void *v, void *p), void *p);
int BinaryTree_traverse_nodes(BinaryTree *T, int (*f)(BinaryTreeNode *n, void *p), void *p);
int BinaryTree_traverse_nodes_r(BinaryTree *T, int (*f)(BinaryTreeNode *n, void *p), void *p);

#define BinaryTree_count(T) (T)->count

#endif // _binary_tree_h
