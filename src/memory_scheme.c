#include <assert.h>
#include <string.h>

#include "check.h"
#include "log.h"
#include "memory_scheme.h"

/***************************************************************************************
 *
 * int generate_memory_scheme(mscheme *s, t_intf *kt, t_intf *vt);
 *
 * Parameters:
 * mscheme *s   The address of the memory scheme to write to.
 * t_intf *kt   Type interface for keys.
 * t_intf *vt   Type interface for values. Can be NULL.
 *
 * Description:
 * Decide whether to store keys and values internally or externally and save the
 * decision together with both type interfaces in a mscheme at the address
 * pointed to by s. The key type is mandatory, the value type can be omitted
 * if single elements are going to be stored in the container. Both types must
 * contain at least a size. The mscheme can then be used by the setters and
 * getters defined in memory_scheme.h to obtain information on the data layout.
 *
 * The internal storage for pairs in a mapping is assumed to be no greater than 2 *
 * sizeof(char*). For instance, assuming the size of a pointer is 8 bytes, we have the
 * following decisions depending on the sizes of keys and values:
 *
 * key + value <= 16 -> store both internally
 * key + value > 16
 *      key <= 8     -> store key internally, value externally
 *      value <= 8   -> store key externally, value internally
 *      both > 8     -> store both externally
 *
 * Return values:
 * 0 on success, -1 on error
 *
 **************************************************************************************/

int generate_memory_scheme(mscheme *s, t_intf *kt, t_intf *vt)
{
    log_call("s=%p, kt=%p, vt=%p", s, kt, vt);

    check(s != NULL, "s == NULL");
    check(kt && kt->size > 0, "kt == NULL or kt->size == 0");
    check(vt && vt->size > 0, "vt == NULL or vt->size == 0");

    s->key_type = kt;
    s->value_type = vt;
    s->key_external = s->value_external = s->value_offset = 0;

    if (vt) {
        if (t_size(kt) + t_size(vt) <= 2 * sizeof(char*)) {
            s->value_offset = t_size(kt);
        } else {
            if (t_size(kt) <= sizeof(char*)) {
                s->value_external = 1;
                s->value_offset = t_size(kt);
            } else if (t_size(vt) <= sizeof(char*)) {
                s->key_external = 1;
                s->value_offset = sizeof(char*);
            } else {
                s->key_external = 1;
                s->value_external = 1;
                s->value_offset = sizeof(char*);
            }
        }
    } else { /* No values. */
        if (t_size(kt) > 2 * sizeof(char*))
            s->key_external = 1;
    }

    return 0;
error:
    return -1;
}

/***************************************************************************************
 *
 * int set_key  (char *data, const mscheme *s, const void *k);
 * int set_value(char *data, const mscheme *s, const void *v);
 *
 * Parameters:
 * char *data           The memory block to write to.
 * const mscheme *s     The memory scheme to use.
 * const void *k        The key to copy.
 * const void *v        The value to copy.
 *
 * Description:
 * Set the key/value part of data to the value pointed to by k/v according to the memory
 * layout specified in s; that is, by copying it either directly into data, or by
 * copying it to the heap and storing a pointer to it in data.
 *
 * Note that we assume that no key/value is stored in data. We don't destroy an existing
 * key/value here if there is one.  It's the caller's response to ensure that we don't
 * overwrite a key/value without properly destroying it.
 *
 * Return values:
 * 0 on success, -1 on error.
 *
 **************************************************************************************/

int set_key(char *data, const mscheme *s, const void *k)
{
    log_call("data=%p, s=%p, k=%p", data, s, k);

    check(data != NULL, "data == NULL");
    check(s && s->key_type, "s == NULL or s->key_type == NULL");
    check(k != NULL, "k == NULL");

    if (s->key_external) {
        void **addr = (void**)data;
        assert(*addr == NULL);
        *addr = t_allocate(s->key_type, 1);
        check_alloc(*addr);
        t_copy(s->key_type, *addr, k);
    }

    else {
        t_copy(s->key_type, data, k);
    }

    return 0;
error:
    /* We check the allocation last, so there's no memory to free here. */
    return -1;
}

int set_value(char *data, const mscheme *s, const void *v)
{
    log_call("data=%p, s=%p, v=%p", data, s, v);

    check(data != NULL, "data == NULL");
    check(s && s->value_type, "s == NULL or s->value_type == NULL");
    check(s->value_offset, "s->value_offset == 0");
    check(v != NULL, "v == NULL");

    if (s->value_external) {
        void **addr = (void**)(data + s->value_offset);
        assert(*addr == NULL);
        *addr = t_allocate(s->value_type, 1);
        check_alloc(*addr);
        t_copy(s->value_type, *addr, v);
    }

    else {
        t_copy(s->value_type, data + s->value_offset, v);
    }

    return 0;
error:
    /* We check the allocation last, so there's no memory to free here. */
    return -1;
}

/***************************************************************************************
 *
 * void destroy_key(char *data, const mscheme *s);
 * void destroy_value(char *data, const mscheme *s);
 *
 * Destroy the key/value saved in data, freeing any associated memory. Zero out the
 * space where it was saved. It's assumed that there actually is a key/value.
 *
 **************************************************************************************/

void destroy_key(char *data, const mscheme *s)
{
    log_call("data=%p, s=%p", data, s);
    assert(data && s->key_type);

    if (s->key_external) {
        void **addr = (void**)data;
        assert(*addr != NULL);
        t_destroy(s->key_type, *addr);
        free(*addr);
        *addr = NULL;
    } else {
        t_destroy(s->key_type, data);
        memset(data, 0, t_size(s->key_type));
    }
}

void destroy_value(char *data, const mscheme *s)
{
    log_call("data=%p, s=%p", data, s);
    assert(data && s->value_type);

    if (s->value_external) {
        void **addr = (void**)(data + s->value_offset);
        assert(*addr != NULL);
        t_destroy(s->value_type, *addr);
        free(*addr);
        *addr = NULL;
    } else {
        t_destroy(s->value_type, data + s->value_offset);
        memset(data + s->value_offset, 0, t_size(s->value_type));
    }
}

/***************************************************************************************
 *
 * void *get_key    (const char *data, const mscheme *s);
 * void *get_address(const char *data, const mscheme *s);
 *
 * Return the memory address of the key/value stored in data, which is either inside
 * data or on the heap. It's assumed that a key was previously saved in data.
 *
 **************************************************************************************/

void *get_key(char *data, const mscheme *s)
{
    assert(data);
    return s->key_external ? *(void**)data : (void*)data;
}

void *get_value(char *data, const mscheme *s)
{
    assert(data);
    return s->value_external
        ? *(void**)(data + s->value_offset)
        : (void*)(data + s->value_offset);
}
