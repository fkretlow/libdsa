#include <string.h>
#include "node_data.h"

/***************************************************************************************
 *
 * int MappingData_set_key(MappingData *d, int external,
 *                         TypeInterface *key_type,
 *                         const void *k);
 *
 * Set the key in the key-value pair stored in d to the key pointed to by k by copying k
 * either directly into internal field of the MappingData union, or by copying it to the
 * heap and storing a pointer to it in the external field.
 *
 * Since there is no use case (except for reusing nodes for optimization) where it makes
 * sense to overwrite an existing key in a mapping, we assume that no key is stored in
 * n.  Note that we don't destroy an existing key here if there is one.  It's the
 * caller's response to ensure that we don't overwrite a key without properly destroying
 * it.
 *
 * Return values: 0 on success
 *               -1 on error
 *
 **************************************************************************************/

int MappingData_set_key(MappingData *d, int external,
                        TypeInterface *key_type,
                        const void *k)
{
    check_ptr(d);
    check_ptr(key_type);
    check_ptr(k);

    if (external) {
        assert(d->external.key == NULL);
        d->external.key = TypeInterface_allocate(key_type, 1);
        check_alloc(d->external.key);
        TypeInterface_copy(key_type, d->external.key, k);
    }

    else {
        TypeInterface_copy(key_type, d->internal.data, k);
    }

    return 0;
error:
    /* The last check happens before the key is copied, so we still only have raw
     * memory. */
    if (d->external.key) free(d->external.key);
    return -1;
}

/***************************************************************************************
 *
 * void MappingData_get_key(MappingData *d, int external,
 *                          TypeInterface *key_type,
 *                          void *key_out);
 *
 * Copy the key saved in d to the address pointed to by key_out. It's assumed that there
 * is enough space and that there's a key stored in d.
 *
 **************************************************************************************/

void MappingData_get_key(MappingData *d, int external,
                         TypeInterface *key_type,
                         void *key_out)
{
    if (external) {
        TypeInterface_copy(key_type, key_out, d->external.key);
    } else {
        TypeInterface_copy(key_type, key_out, d->internal.data)
    }
}

/***************************************************************************************
 *
 * void MappingData_destroy_key(MappingData *d, int external, TypeInterface *key_type);
 *
 * Destroy the key saved in d, freeing any associated memory. Zero out the space where
 * it was saved in d. It's assumed that there actually is a key.
 *
 **************************************************************************************/

void MappingData_destroy_key(MappingData *d, int external, TypeInterface *key_type)
{
    assert(d && key_type);
    if (external) {
        TypeInterface_destroy(key_type, d->external.key);
        free(d->external.key);
        d->external.key = NULL;
    } else {
        TypeInterface_destroy(key_type, d->internal.data);
        memset(d->internal.data, 0, TypeInterface_size(key_type));
    }
}

/***************************************************************************************
 *
 * void *MappingData_key_address(MappingData *d, int external);
 *
 * Return the memory address of the key stored in d, which is either inside d or on the
 * heap. It's assumed that a key was previously saved in d.
 *
 **************************************************************************************/

void *MappingData_key_address(MappingData *d, int external)
{
    assert(d);
    if (external) return d->external.key;
    else          return d->internal.data;
}

int MappingData_set_value(MappingData *d, int external,
                          TypeInterface *key_type, TypeInterface *value_type,
                          const void *v);

int MappingData_get_value(MappingData *d, int external,
                          TypeInterface *key_type, TypeInterface *value_type,
                          void *k_out);

int MappingData_destroy_value(MappingData *d, int external,
                              TypeInterface *key_type, TypeInterface *value_type);

void *MappingData_value_address(MappingData *d, int external, TypeInterface *key_type);
