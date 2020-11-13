#include "binary_tree.h"

/***************************************************************************************
 *
 * int BinaryTree_initialize(BinaryTree *T,
 *                           TypeInterface *key_type,
 *                           TypeInterface *value_type);
 *
 * Initialize T, which is assumed to be a pointer to a memory location with enough space
 * for a binary tree. The key type is mandatory, the value type can be NULL if single
 * objects instead of key-value pairs should be stored.
 *
 * Depending on the size of the key and value objects they are stored inside the tree
 * nodes themselves, or on the heap and referenced by pointers.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int BinaryTree_initialize(BinaryTree *T,
                          TypeInterface *key_type,
                          TypeInterface *value_type)
{
    check_ptr(T);
    check_ptr(key_type);
    check(key_type->compare, "No comparison function was given.");

    T->root = NULL;
    T->size = 0;
    T->key_type = key_type;
    T->value_type = value_type;
    T->flags.memory_scheme = MappingData_generate_memory_scheme(key_type, value_type);

    return 0;
error:
    return -1;
}
