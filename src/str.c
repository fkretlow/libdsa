#include <assert.h>
#include <string.h>

#include "container_tools.h"
#include "check.h"
#include "str.h"
#include "type_interface.h"


int str_initialize(str *s)
{
    if (s) memset(s, 0, sizeof(*s));
    return 0;
}

void str_destroy(void *sp)
{
    str *s = sp;
    if (s) {
        if (s->data_external) {
            free(s->data.external.data);
            s->data_external = 0;
        }
    }
}

str *str_new(void) { return calloc(1, sizeof(str)); }

void str_delete(str *s)
{
    if (s) {
        str_destroy(s);
        free(s);
    }
}

/* Allocate memory for at least `capacity` characters.
 * Does not shrink the internal storage. */
int str_reserve(str *s, const size_t capacity)
{
    check_ptr(s);

    if (capacity < STR_INTERNAL_CAPACITY - 1
            || (s->data_external && capacity < s->data.external.capacity))
        return 0;

    size_t c = 2;
    while (c < capacity) c <<= 1;

    char *data;
    if (s->data_external) {
        data = realloc(s->data.external.data, c * sizeof(char));
        check_alloc(data);
    } else {
        data = malloc(c * sizeof(char));
        check_alloc(data);
        memmove(data, s->data.internal.data, s->length + 1);
    }
    s->data.external.data = data;
    s->data.external.capacity = c;
    s->data_external = 1;

    return 0;
error:
    if (data) free(data);
    return -1;
}

/* Shrink the internal storage if and as possible. Does not necessarily shrink
 * it to the exact size needed. */
int str_shrink_to_fit(str *s)
{
    check_ptr(s);

    if (str_capacity(s) <= s->length + 1) return 0;

    if (s->length < STR_INTERNAL_CAPACITY) {
        if (s->data_external) {
            char *temp = malloc((s->length + 1) * sizeof(char));
            check_alloc(temp);
            memmove(temp, s->data.external.data, s->length + 1);
            free(s->data.external.data);
            s->data.external.capacity = 0;
            memmove(s->data.internal.data, temp, s->length + 1);
            s->data_external = 0;
            free(temp);
        } else {
            /* Nothing to do. */
        }
    } else { /* s->length >= STR_INTERNAL_CAPACITY */
        assert(s->data_external);
        size_t c = 2;
        while (c < s->length + 1) c <<= 1;

        char *data = realloc(s->data.external.data, c * sizeof(char));
        check_alloc(data);
        s->data.external.data = data;
        s->data.external.capacity = c;
    }

    return 0;
error:
    return -1;
}

void str_clear(str *s)
{
    if (s) {
        if (s->data_external) {
            free(s->data.external.data);
            s->data_external = 0;
        }
        s->length = 0;
        s->data.internal.data[0] = 0;
    }
}

str *str_copy(const str *src)
{
    str *s = NULL;
    check_ptr(src);

    s = str_new();
    check(s != NULL, "Failed to allocate new str.");

    int rc = str_assign(s, src);
    check(rc == 0, "Failed to assign to str copy.");

    return s;
error:
    if (s) str_delete(s);
    return NULL;
}

void str_copy_to(void *dest, const void *src)
{
    str_initialize(dest);
    str_assign(dest, src);
}

int str_assign(str *dest, const str *src)
{
    check_ptr(dest);
    check_ptr(src);

    int rc = str_reserve(dest, src->length + 1);
    check(rc == 0, "Failed to allocate internal storage.");

    const char *src_data = str_data(src);
    char *dest_data = str_data(dest);
    memmove(dest_data, src_data, src->length + 1);
    dest->length = src->length;

    return 0;
error:
    return -1;
}

int str_assign_cstr(str *dest, const char *cstr)
{
    check_ptr(dest);
    check_ptr(cstr);

    size_t cstrlen = strnlen(cstr, STR_MAX_CAPACITY);
    int rc = str_reserve(dest, cstrlen + 1);
    check(rc == 0, "Failed to allocate internal storage.");

    char *dest_data = str_data(dest);
    memmove(dest_data, cstr, cstrlen);
    dest_data[cstrlen] = 0;
    dest->length = cstrlen;

    return 0;
error:
    return -1;
}

str *str_from_cstr(const char *cstr)
{
    str *s = NULL;
    check_ptr(cstr);

    s = str_new();
    check(s != NULL, "Failed to make new str.");

    int rc = str_assign_cstr(s, cstr);
    check(rc == 0, "Failed to assign to new string.");

    return s;
error:
    if (s) str_delete(s);
    return NULL;
}

int str_compare(const void *a, const void *b)
{
    const str *s1 = a;
    const str *s2 = b;
    return strncmp(str_data(s1),
                   str_data(s2),
                   s1->length > s2->length ? s1->length : s2->length);
}

int str_append(str *s1, const str *s2)
{
    check_ptr(s1);
    check_ptr(s2);

    size_t size = s1->length + s2->length;
    size_t capacity = str_capacity(s1);
    while (capacity < size + 1) capacity <<= 1;
    int rc = str_reserve(s1, capacity);
    check(rc == 0, "Failed to ensure there's enough internal storage.");

    char *s1_data = str_data(s1);
    const char *s2_data = str_data(s2);
    memmove(s1_data + s1->length, s2_data, s2->length);
    s1->length = size;
    s1_data[size] = 0;

    return 0;
error:
    return -1;
}

int str_append_cstr(str *s, const char *cstr)
{
    check_ptr(s);
    check_ptr(cstr);

    size_t cstrlen = strnlen(cstr, STR_MAX_CAPACITY);
    size_t size = s->length + cstrlen;
    size_t capacity = str_capacity(s);
    while (capacity < size + 1) capacity <<= 1;
    int rc = str_reserve(s, capacity);
    check(rc == 0, "Failed to allocate internal storage.");

    char *s_data = str_data(s);
    memmove(s_data + s->length, cstr, cstrlen);
    s->length = size;
    s_data[size] = 0;

    return 0;
error:
    return -1;
}

int str_push_back(str *s, const char c)
{
    check_ptr(s);

    if (str_capacity(s) <= s->length + 1) {
        int rc = str_reserve(s, s->length + 2);
        check(rc == 0, "Failed to allocate internal storage.");
    }

    char *data = str_data(s);
    data[s->length] = c;
    ++s->length;
    data[s->length] = 0;

    return 0;
error:
    return -1;
}

int str_pop_back(str *s, char *out)
{
    check_ptr(s);
    check(s->length > 0, "Attempt to pop_back from empty string.");

    char *data = str_data(s);
    --s->length;
    if (out) *out = data[s->length];
    data[s->length] = 0;

    return 0;
error:
    return -1;
}

str *str_concat(const str *s1, const str *s2)
{
    str *s = NULL;

    check_ptr(s1);
    check_ptr(s2);

    s = str_copy(s1);
    check(s != NULL, "Failed to copy the first str.");
    int rc = str_append(s, s2);
    check(rc == 0, "Failed to append the second str.");

    return s;
error:
    if (s) str_delete(s);
    return NULL;
}

uint32_t str_hash(const void *s)
{
    char *data = str_data((str*)s);
    return jenkins_hash(data, ((str *)s)->length);
}

void str_print(FILE *stream, const void *s)
{
    char *data = str_data((str *)s);
    fprintf(stream, "%s", data);
}
