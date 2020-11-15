#include <assert.h>
#include <string.h>

#include "container_tools.h"
#include "debug.h"
#include "str.h"
#include "type_interface.h"


int String_initialize(String *s)
{
    if (s) memset(s, 0, sizeof(*s));
    return 0;
}

void String_destroy(void *sp)
{
    String *s = sp;
    if (s) {
        if (s->storage_allocated) {
            free(s->data.external.data);
            s->storage_allocated = 0;
        }
    }
}

String *String_new(void) { return calloc(1, sizeof(String)); }

void String_delete(String *s)
{
    if (s) {
        String_destroy(s);
        free(s);
    }
}

/* Allocate memory for at least `capacity` characters.
 * Does not shrink the internal storage. */
int String_reserve(String *s, const size_t capacity)
{
    check_ptr(s);

    if (capacity < STRING_ALLOC_THRESHOLD - 1
            || (s->storage_allocated && capacity < s->data.external.capacity))
        return 0;

    size_t c = STRING_ALLOC_THRESHOLD;
    while (c < capacity) c <<= 1;

    char *data;
    if (s->storage_allocated) {
        data = realloc(s->data.external.data, c * sizeof(char));
        check_alloc(data);
    } else {
        data = malloc(c * sizeof(char));
        check_alloc(data);
        memmove(data, s->data.internal.data, s->size + 1);
    }
    s->data.external.data = data;
    s->data.external.capacity = c;
    s->storage_allocated = true;

    return 0;
error:
    if (data) free(data);
    return -1;
}

/* Shrink the internal storage if and as possible. Does not necessarily shrink
 * it to the exact size needed. */
int String_shrink_to_fit(String *s)
{
    check_ptr(s);

    if (String_capacity(s) <= s->size + 1) return 0;

    if (s->size < STRING_ALLOC_THRESHOLD) {
        if (s->storage_allocated) {
            char *temp = malloc((s->size + 1) * sizeof(char));
            check_alloc(temp);
            memmove(temp, s->data.external.data, s->size + 1);
            free(s->data.external.data);
            s->data.external.capacity = 0;
            memmove(s->data.internal.data, temp, s->size + 1);
            s->storage_allocated = false;
            free(temp);
        } else {
            /* Nothing to do. */
        }
    } else { /* s->size >= STRING_ALLOC_THRESHOLD */
        assert(s->storage_allocated);
        size_t c = STRING_ALLOC_THRESHOLD;
        while (c < s->size + 1) c <<= 1;

        char *data = realloc(s->data.external.data, c * sizeof(char));
        check_alloc(data);
        s->data.external.data = data;
        s->data.external.capacity = c;
    }

    return 0;
error:
    return -1;
}

void String_clear(String *s)
{
    if (s) {
        if (s->storage_allocated) {
            free(s->data.external.data);
            s->storage_allocated = false;
        }
        s->size = 0;
        s->data.internal.data[0] = 0;
    }
}

String *String_copy(const String *src)
{
    String *s = NULL;
    check_ptr(src);

    s = String_new();
    check(s != NULL, "Failed to allocate new String.");

    int rc = String_assign(s, src);
    check(rc == 0, "Failed to assign to String copy.");

    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

void String_copy_to(void *dest, const void *src)
{
    String_initialize(dest);
    String_assign(dest, src);
}

int String_assign(String *dest, const String *src)
{
    check_ptr(dest);
    check_ptr(src);

    int rc = String_reserve(dest, src->size + 1);
    check(rc == 0, "Failed to allocate internal storage.");

    const char *src_data = String_data(src);
    char *dest_data = String_data(dest);
    memmove(dest_data, src_data, src->size + 1);
    dest->size = src->size;

    return 0;
error:
    return -1;
}

int String_assign_cstr(String *dest, const char *cstr)
{
    check_ptr(dest);
    check_ptr(cstr);

    size_t cstrlen = strnlen(cstr, STRING_MAX_SIZE);
    int rc = String_reserve(dest, cstrlen + 1);
    check(rc == 0, "Failed to allocate internal storage.");

    char *dest_data = String_data(dest);
    memmove(dest_data, cstr, cstrlen);
    dest_data[cstrlen] = 0;
    dest->size = cstrlen;

    return 0;
error:
    return -1;
}

String *String_from_cstr(const char *cstr)
{
    String *s = NULL;
    check_ptr(cstr);

    s = String_new();
    check(s != NULL, "Failed to make new String.");

    int rc = String_assign_cstr(s, cstr);
    check(rc == 0, "Failed to assign to new string.");

    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

int String_compare(const void *a, const void *b)
{
    const String *s1 = a;
    const String *s2 = b;
    return strncmp(String_data(s1),
                   String_data(s2),
                   _max(s1->size, s2->size));
}

int String_append(String *s1, const String *s2)
{
    check_ptr(s1);
    check_ptr(s2);

    size_t size = s1->size + s2->size;
    size_t capacity = String_capacity(s1);
    while (capacity < size + 1) capacity <<= 1;
    int rc = String_reserve(s1, capacity);
    check(rc == 0, "Failed to ensure there's enough internal storage.");

    char *s1_data = String_data(s1);
    const char *s2_data = String_data(s2);
    memmove(s1_data + s1->size, s2_data, s2->size);
    s1->size = size;
    s1_data[size] = 0;

    return 0;
error:
    return -1;
}

int String_append_cstr(String *s, const char *cstr)
{
    check_ptr(s);
    check_ptr(cstr);

    size_t cstrlen = strnlen(cstr, STRING_MAX_SIZE);
    size_t size = s->size + cstrlen;
    size_t capacity = String_capacity(s);
    while (capacity < size + 1) capacity <<= 1;
    int rc = String_reserve(s, capacity);
    check(rc == 0, "Failed to allocate internal storage.");

    char *s_data = String_data(s);
    memmove(s_data + s->size, cstr, cstrlen);
    s->size = size;
    s_data[size] = 0;

    return 0;
error:
    return -1;
}

int String_push_back(String *s, const char c)
{
    check_ptr(s);

    if (String_capacity(s) <= s->size + 1) {
        int rc = String_reserve(s, s->size + 2);
        check(rc == 0, "Failed to allocate internal storage.");
    }

    char *data = String_data(s);
    data[s->size] = c;
    ++s->size;
    data[s->size] = 0;

    return 0;
error:
    return -1;
}

int String_pop_back(String *s, char *out)
{
    check_ptr(s);
    check(s->size > 0, "Attempt to pop_back from empty string.");

    char *data = String_data(s);
    --s->size;
    if (out) *out = data[s->size];
    data[s->size] = 0;

    return 0;
error:
    return -1;
}

String *String_concat(const String *s1, const String *s2)
{
    String *s = NULL;

    check_ptr(s1);
    check_ptr(s2);

    s = String_copy(s1);
    check(s != NULL, "Failed to copy the first String.");
    int rc = String_append(s, s2);
    check(rc == 0, "Failed to append the second String.");

    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

uint32_t String_hash(const void *s)
{
    char *data = String_data((String*)s);
    return jenkins_hash(data, ((String *)s)->size);
}

void String_printf(FILE *stream, const void *s)
{
    char *data = String_data((String *)s);
    fprintf(stream, "%s", data);
}
