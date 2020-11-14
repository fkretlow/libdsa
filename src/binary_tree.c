#include <stdlib.h>

#include "binary_tree.h"

/***************************************************************************************
 *
 * int BinaryTree_initialize(BinaryTree *T, int balancing_strategy,
 *                           TypeInterface *key_type, TypeInterface *value_type);
 *
 * Initialize T, which is assumed to be a pointer to a memory location with enough space
 * for a binary tree. The key type is mandatory, the value type can be NULL if single
 * objects instead of key-value pairs should be stored. The balancing strategy must be
 * one of 0 (no balancing), 1 (red-black tree), or 2 (AVL tree).
 *
 * Depending on the size of the key and value objects they will be stored inside the
 * tree nodes themselves, or on the heap and referenced by pointers.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int BinaryTree_initialize(BinaryTree *T, int balancing_strategy,
                          TypeInterface *key_type,
                          TypeInterface *value_type)
{
    check_ptr(T);
    check(0 <= balancing_strategy && balancing_strategy <= 2, "Bad strategy.");
    check_ptr(key_type);
    check(key_type->compare, "No comparison function was given.");

    T->root = NULL;
    T->count = 0;
    T->balancing_strategy = balancing_strategy;

    int rc = MemoryScheme_initialize(&T->memory_scheme, key_type, value_type);
    check(rc == 0, "Failed to initialize memory scheme.");

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * BinaryTree *BinaryTree_new(int balancing_strategy,
 *                            TypeInterface *key_type, TypeInterface *value_type);
 *
 * Allocate a binary tree and initialize it. See BinaryTree_initialize for further
 * details.
 *
 * Return values: T    on success
 *                NULL on error
 *
 **************************************************************************************/

BinaryTree *BinaryTree_new(int balancing_strategy,
                           TypeInterface *key_type, TypeInterface *value_type)
{
    BinaryTree *T = calloc(1, sizeof(*T));
    check_alloc(T);

    int rc = BinaryTree_initialize(T, balancing_strategy, key_type, value_type);
    check(rc == 0, "Failed to initialize new binary tree.");

    return T;
error:
    return NULL;
}
