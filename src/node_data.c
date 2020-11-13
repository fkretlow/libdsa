#include <assert.h>
#include <string.h>

#include "node_data.h"

/***************************************************************************************
 *
 * int MappingData_set_key(MappingData *data, const int external,
 *                         const TypeInterface *key_type,
 *                         const void *key);
 *
 * Set the key in the key-value pair stored in data to the object pointed to by key by
 * copying it either directly into internal field of the MappingData union, or by
 * copying it to the heap and storing a pointer to it in the external field.
 *
 * Since there is no use case (except for reusing nodes for optimization) where it makes
 * sense to overwrite an existing key in a mapping, we assume that no key is stored in
 * data.  Note that we don't destroy an existing key here if there is one.  It's the
 * caller's response to ensure that we don't overwrite a key without properly destroying
 * it.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int MappingData_set_key(MappingData *data, const int external,
                        const TypeInterface *key_type,
                        const void *key)
{
    check_ptr(data);
    check_ptr(key_type);
    check_ptr(key);

    if (external) {
        assert(data->external.key == NULL);
        data->external.key = TypeInterface_allocate(key_type, 1);
        check_alloc(data->external.key);
        TypeInterface_copy(key_type, data->external.key, key);
    }

    else {
        TypeInterface_copy(key_type, data->internal.data, key);
    }

    return 0;
error:
    /* The last check happens before the key is copied, so we still only have raw
     * memory. */
    if (data->external.key) free(data->external.key);
    return -1;
}

/***************************************************************************************
 *
 * void MappingData_get_key(const MappingData *data, const int external,
 *                          const TypeInterface *key_type,
 *                          void *key_out);
 *
 * Copy the key saved in data to the address pointed to by key_out. It's assumed that
 * there is enough space and that there's a key stored in data.
 *
 **************************************************************************************/

void MappingData_get_key(const MappingData *data, const int external,
                         const TypeInterface *key_type,
                         void *key_out)
{
    if (external) {
        TypeInterface_copy(key_type, key_out, data->external.key);
    } else {
        TypeInterface_copy(key_type, key_out, data->internal.data);
    }
}

/***************************************************************************************
 *
 * void MappingData_destroy_key(MappingData *data, const int external,
 *                              const TypeInterface *key_type);
 *
 * Destroy the key saved in data, freeing any associated memory. Zero out the space
 * where it was saved in data. It's assumed that there actually is a key.
 *
 **************************************************************************************/

void MappingData_destroy_key(MappingData *data, const int external,
                             const TypeInterface *key_type)
{
    assert(data && key_type);
    if (external) {
        assert(data->external.key != NULL);
        TypeInterface_destroy(key_type, data->external.key);
        free(data->external.key);
        data->external.key = NULL;
    } else {
        TypeInterface_destroy(key_type, data->internal.data);
        memset(data->internal.data, 0, TypeInterface_size(key_type));
    }
}

/***************************************************************************************
 *
 * void *MappingData_key_address(const MappingData *data, const int external);
 *
 * Return the memory address of the key stored in data, which is either inside data or
 * on the
 * heap. It's assumed that a key was previously saved in data.
 *
 **************************************************************************************/

void *MappingData_key_address(MappingData *data, const int external)
{
    assert(data);
    if (external) return data->external.key;
    else          return data->internal.data;
}

/***************************************************************************************
 *
 * int MappingData_set_value(MappingData *data, const int external,
 *                           const TypeInterface *key_type,
 *                           const TypeInterface *value_type,
 *                           const void *value);
 *
 * Set the value in the key-value pair stored in data to the object pointed to by value
 * by copying it either directly into internal field of the MappingData union, or by
 * copying it to the heap and storing a pointer to it in the external field.
 *
 * It's common to update a value in a mapping, but we don't destroy an existing value
 * here.  It's the caller's response to ensure that we don't overwrite a value without
 * properly destroying it first.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int MappingData_set_value(MappingData *data, const int external,
                          const TypeInterface *key_type,
                          const TypeInterface *value_type,
                          const void *value)
{
    check_ptr(data);
    check_ptr(key_type);
    check_ptr(value_type);
    check_ptr(value);

    if (external) {
        assert(data->external.value == NULL);
        data->external.value = TypeInterface_allocate(value_type, 1);
        check_alloc(data->external.value);
        TypeInterface_copy(value_type, data->external.value, value);
    }

    else {
        TypeInterface_copy(value_type,
                           data->internal.data + TypeInterface_size(key_type),
                           value);
    }

    return 0;
error:
    /* The last check happens before the value is copied, so we still only have raw
     * memory. */
    if (data->external.value) free(data->external.value);
    return -1;
}

/***************************************************************************************
 *
 * void MappingData_get_value(const MappingData *data, const int external,
 *                            const TypeInterface *key_type,
 *                            const TypeInterface *value_type,
 *                            void *value_out);
 *
 * Copy the value saved in data to the address pointed to by value_out. It's assumed
 * that there is enough space and that there's a value stored in data.
 *
 **************************************************************************************/

void MappingData_get_value(const MappingData *data, const int external,
                           const TypeInterface *key_type,
                           const TypeInterface *value_type,
                           void *value_out)
{
    if (external) {
        TypeInterface_copy(value_type, value_out, data->external.value);
    } else {
        TypeInterface_copy(value_type, value_out,
                           data->internal.data + TypeInterface_size(key_type));
    }
}

/***************************************************************************************
 *
 * void MappingData_destroy_value(MappingData *data, const int external,
 *                                const TypeInterface *key_type,
 *                                const TypeInterface *value_type);
 *
 * Destroy the value saved in data, freeing any associated memory. Zero out the space
 * where it was saved in data. It's assumed that there actually is a value.
 *
 **************************************************************************************/

void MappingData_destroy_value(MappingData *data, const int external,
                               const TypeInterface *key_type,
                               const TypeInterface *value_type)
{
    assert(data && value_type && key_type);
    if (external) {
        assert(data->external.value != NULL);
        TypeInterface_destroy(value_type, data->external.value);
        free(data->external.value);
        data->external.value = NULL;
    } else {
        TypeInterface_destroy(value_type,
                              data->internal.data + TypeInterface_size(key_type));
        memset(data->internal.data + TypeInterface_size(key_type),
               0, TypeInterface_size(value_type));
    }
}

/***************************************************************************************
 *
 * void *MappingData_value_address(const MappingData *data, const int external,
 *                                 const TypeInterface *key_type);
 *
 * Return the memory address of the value stored in data, which is either inside data or
 * on the heap. It's assumed that a value was previously saved in data.
 *
 * Note that key_type is not a typo. We need the size of a key object to compute the
 * offset of the value object in the internal storage.
 *
 **************************************************************************************/

void *MappingData_value_address(MappingData *data, const int external,
                                const TypeInterface *key_type)
{
    assert(data);
    if (external) return data->external.value;
    else          return data->internal.data + TypeInterface_size(key_type);
}
