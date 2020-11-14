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
 * Depending on the size of the key and value objects they are stored inside the tree
 * nodes themselves, or on the heap and referenced by pointers.
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
    T->memory_scheme = MappingData_make_memory_scheme(key_type, value_type);
    T->balancing_strategy = balancing_strategy;

    return 0;
error:
    return -1;
}
