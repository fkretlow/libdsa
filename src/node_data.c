#include <assert.h>
#include <string.h>

#include "node_data.h"

/***************************************************************************************
 *
 * int MemoryScheme_initialize(MemoryScheme *scheme,
                               TypeInterface *key_type, TypeInterface *value_type);

 * Decide whether to store keys and values internally or externally and store the
 * decisions together with both type interfaces in the struct MemoryScheme pointed to by
 * scheme. The key type is mandatory, the value type can be omitted (be NULL) if no
 * values are going to be stored in the container.
 *
 * The internal storage for pairs in a mapping is assumed to be no greater than 2 *
 * sizeof(void*). For instance, assuming the size of a pointer is 8 bytes on a typical
 * 64bit machine, this leads to the following decisions depending on the sizes of key
 * and value objects:
 *
 * key + value <= 16 -> store both internally
 * key + value > 16
 *      key <= 8     -> store key internally, value externally
 *      value <= 8   -> store key externally, value internally
 *      both > 8     -> store both externally
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int MemoryScheme_initialize(MemoryScheme *scheme,
                            TypeInterface *key_type, TypeInterface *value_type)
{
    check_ptr(key_type);
    scheme->key_type = key_type;
    scheme->value_type = value_type;
    scheme->key_external = scheme->value_external = scheme->value_offset = 0;

    if (value_type) {
        if (TypeInterface_size(key_type) + TypeInterface_size(value_type)
                <= 2 * sizeof(char*)) {
            scheme->value_offset = TypeInterface_size(key_type);
        } else {
            if (TypeInterface_size(key_type) <= sizeof(char*)) {
                scheme->value_external = 1;
                scheme->value_offset = TypeInterface_size(key_type);
            } else if (TypeInterface_size(value_type) <= sizeof(char*)) {
                scheme->key_external = 1;
                scheme->value_offset = sizeof(char*);
            } else {
                scheme->key_external = 1;
                scheme->value_external = 1;
                scheme->value_offset = sizeof(char*);
            }
        }
    } else {
        if (TypeInterface_size(key_type) <= 2 * sizeof(char*)) {
            /* Nothing to do. */
        } else {
            scheme->key_external = 1;
        }
    }

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * int MappingData_set_key(char *data, const MemoryScheme *scheme, const void *key);
 *
 * Set the key in the key-value pair stored in data to the object pointed to by key by
 * copying it either directly into data field, or by copying it to the heap and storing
 * a pointer to it in the data field.
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

int MappingData_set_key(char *data, const MemoryScheme *scheme, const void *key)
{
    check_ptr(data);
    check_ptr(scheme->key_type);
    check_ptr(key);

    if (scheme->key_external) {
        void **key_address = (void**)data;
        assert(*key_address == NULL);
        *key_address = TypeInterface_allocate(scheme->key_type, 1);
        check_alloc(*key_address);
        TypeInterface_copy(scheme->key_type, *key_address, key);
    }

    else {
        TypeInterface_copy(scheme->key_type, data, key);
    }

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * void MappingData_get_key(const char *data, const MemoryScheme *scheme,
 *                          void *key_out);
 *
 * Copy the key saved in data to the address pointed to by key_out. It's assumed that
 * there is enough space and that there's a key stored in data.
 *
 **************************************************************************************/

void MappingData_get_key(const char *data, const MemoryScheme *scheme, void *key_out)
{
    if (scheme->key_external) {
        void **key_address = (void**)data;
        assert(*key_address != NULL);
        TypeInterface_copy(scheme->key_type, key_out, *key_address);
    } else {
        TypeInterface_copy(scheme->key_type, key_out, data);
    }
}

/***************************************************************************************
 *
 * void MappingData_destroy_key(char *data, const MemoryScheme *scheme);
 *
 * Destroy the key saved in data, freeing any associated memory. Zero out the space
 * where it was saved in data. It's assumed that there actually is a key.
 *
 **************************************************************************************/

void MappingData_destroy_key(char *data, const MemoryScheme *scheme)
{
    assert(data && scheme->key_type);
    if (scheme->key_external) {
        void **key_address = (void**)data;
        assert(*key_address != NULL);
        TypeInterface_destroy(scheme->key_type, *key_address);
        free(*key_address);
        *key_address = NULL;
    } else {
        TypeInterface_destroy(scheme->key_type, data);
        memset(data, 0, TypeInterface_size(scheme->key_type));
    }
}

/***************************************************************************************
 *
 * void *MappingData_key_address(const char *data, const MemoryScheme *scheme);
 *
 * Return the memory address of the key stored in data, which is either inside data or
 * on the
 * heap. It's assumed that a key was previously saved in data.
 *
 **************************************************************************************/

void *MappingData_key_address(char *data, const MemoryScheme *scheme)
{
    assert(data);
    return scheme->key_external ? *(void**)data : (void*)data;
}

/***************************************************************************************
 *
 * int MappingData_set_value(char *data, const MemoryScheme *scheme, const void *value);
 *
 * Set the value in the key-value pair stored in data to the object pointed to by value
 * by copying it either directly into data field, or by copying it to the heap and
 * storing a pointer to it in the data field.
 *
 * It's common to update a value in a mapping, but we don't destroy an existing value
 * here.  It's the caller's response to ensure that we don't overwrite a value without
 * properly destroying it first.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int MappingData_set_value(char *data, const MemoryScheme *scheme, const void *value)
{
    check_ptr(data);
    check_ptr(scheme->key_type);
    check_ptr(scheme->value_type);
    check_ptr(value);

    if (scheme->value_external) {
        void **value_address = (void**)(data + scheme->value_offset);
        assert(*value_address == NULL);
        *value_address = TypeInterface_allocate(scheme->value_type, 1);
        check_alloc(*value_address);
        TypeInterface_copy(scheme->value_type, *value_address, value);
    }

    else {
        TypeInterface_copy(scheme->value_type, data+ scheme->value_offset, value);
    }

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * void MappingData_get_value(const char *data, const MemoryScheme *scheme,
 *                            void *value_out);
 *
 * Copy the value saved in data to the address pointed to by value_out. It's assumed
 * that there is enough space and that there's a value stored in data.
 *
 **************************************************************************************/

void MappingData_get_value(const char *data, const MemoryScheme *scheme,
                           void *value_out)
{
    if (scheme->value_external) {
        void **value_address = (void**)(data + scheme->value_offset);
        assert(*value_address != NULL);
        TypeInterface_copy(scheme->value_type, value_out, *value_address);
    } else {
        TypeInterface_copy(scheme->value_type, value_out, data + scheme->value_offset);
    }
}

/***************************************************************************************
 *
 * void MappingData_destroy_value(char *data, const MemoryScheme *scheme);
 *
 * Destroy the value saved in data, freeing any associated memory. Zero out the space
 * where it was saved in data. It's assumed that there actually is a value.
 *
 **************************************************************************************/

void MappingData_destroy_value(char *data, const MemoryScheme *scheme)
{
    assert(data && scheme->value_type);
    if (scheme->value_external) {
        void **value_address = (void**)(data + scheme->value_offset);
        assert(*value_address != NULL);
        TypeInterface_destroy(scheme->value_type, *value_address);
        free(*value_address);
        *value_address = NULL;
    } else {
        TypeInterface_destroy(scheme->value_type, data + scheme->value_offset);
        memset(data + scheme->value_offset, 0, TypeInterface_size(scheme->value_type));
    }
}

/***************************************************************************************
 *
 * void *MappingData_value_address(const char *data, const MemoryScheme *scheme);
 *
 * Return the memory address of the value stored in data, which is either inside data or
 * on the heap. It's assumed that a value was previously saved in data.
 *
 * Note that key_type is not a typo. We need the size of a key object to compute the
 * offset of the value object in the internal storage.
 *
 **************************************************************************************/

void *MappingData_value_address(char *data, const MemoryScheme *scheme)
{
    assert(data);
    return scheme->value_external
        ? *(void**)(data + scheme->value_offset)
        : (void*)(data + scheme->value_offset);
}
