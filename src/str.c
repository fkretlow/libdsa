#include <string.h>

#include "container_tools.h"
#include "debug.h"
#include "str.h"

String String_new(void)
{
    _string *s = calloc(1, sizeof(*s));
    check_alloc(s);

    return s;
error:
    return NULL;
}

void String_delete(String s)
{
    if (s->data) free(s->data);
    free(s);
}

/* Allocate memory for at least `capacity` characters.
 * Does not shrink the internal storage. */
int String_reserve(String s, size_t capacity)
{
    check_ptr(s);

    if (capacity < s->capacity) return 0;

    size_t c = STRING_ALLOC_THRESHOLD;
    while (c < capacity) c <<= 1;
    capacity = c;

    char *data;
    if (s->data) {
        data = realloc(s->data, capacity * sizeof(char));
    } else {
        data = malloc(capacity * sizeof(char));
    }
    check_alloc(data);
    s->data = data;
    s->capacity = capacity;

    return 0;
error:
    return -1;
}

int String_shrink_to_fit(String s)
{
    check_ptr(s);

    if (s->capacity <= s->size + 1) return 0;

    size_t capacity = STRING_ALLOC_THRESHOLD;
    while (capacity < s->size + 1) capacity <<= 1;

    char *data;
    if (s->data) {
        data = realloc(s->data, capacity * sizeof(char));
    } else {
        data = malloc(capacity * sizeof(char));
    }
    check_alloc(data);
    s->data = data;
    s->capacity = capacity;

    return 0;
error:
    return -1;
}

void String_clear(String s)
{
    s->size = 0;
    s->data[0] = '\n';
    String_shrink_to_fit(s);
}

String String_copy(const String src)
{
    String s = NULL;
    check_ptr(src);

    s = String_new();
    check(s != NULL, "Failed to allocate new String.");

    check(!String_assign(s, src), "Failed to assign to String copy.");

    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

int String_assign(String dest, const String src)
{
    check_ptr(dest);
    check_ptr(src);
    check(src->data, "No string data allocated in source string.");

    check(!String_reserve(dest, src->size + 1),
            "Failed to allocate internal storage.");
    memmove(dest->data, src->data, src->size + 1);
    dest->size = src->size;

    return 0;
error:
    return -1;
}

String String_from_cstr(const char *cstr)
{
    String s = NULL;
    check_ptr(cstr);

    s = String_new();
    check(s != NULL, "Failed to make new String.");

    size_t cstrlen = strnlen(cstr, STRING_MAX_SIZE);
    check(!String_reserve(s, cstrlen + 1), "Failed to allocate internal storage.");
    memmove(s->data, cstr, cstrlen + 1);
    s->size = cstrlen;

    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

int String_compare(const String s1, const String s2)
{
    check_ptr(s1);
    check_ptr(s2);
    check(s1->data && s2->data, "No string data allocated.");
    return strncmp(s1->data, s2->data, _max(s1->size, s2->size));
error:
    return -2;
}

int String_append(String s1, const String s2)
{
    check_ptr(s1);
    check_ptr(s2);
    check(s1->data && s2->data, "No string data allocated.");

    size_t size = s1->size + s2->size;
    size_t capacity = s1->capacity;
    while (capacity < size + 1) capacity <<= 1;
    check(!String_reserve(s1, capacity), "Failed to allocate internal storage.");

    memmove(s1->data + s1->size, s2->data, s2->size);
    s1->size = size;
    s1->data[size] = '\0';

    return 0;
error:
    return -1;
}

int String_append_cstr(String s1, const char *cstr)
{
    check_ptr(s1);
    check_ptr(cstr);
    check(s1->data, "No string data allocated.");

    size_t cstrlen = strnlen(cstr, STRING_MAX_SIZE);
    size_t size = s1->size + cstrlen;
    size_t capacity = s1->capacity;
    while (capacity < size + 1) capacity <<= 1;
    check(!String_reserve(s1, capacity), "Failed to allocate internal storage.");

    memmove(s1->data + s1->size, cstr, cstrlen);
    s1->size = size;
    s1->data[size] = '\0';

    return 0;
error:
    return -1;
}

String String_concat(const String s1, const String s2)
{
    String s = NULL;

    check_ptr(s1);
    check_ptr(s2);

    s = String_copy(s1);
    check(s != NULL, "Failed to copy the first String.");
    debug("%s", s->data);
    check(!String_append(s, s2), "Failed to append the second String.");
    debug("%s", s->data);

    return s;
error:
    return NULL;
}

uint32_t String_hash(const String s)
{
    return jenkins_hash(s->data, s->size);
}
