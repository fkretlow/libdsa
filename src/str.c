#include <string.h>

#include "container_tools.h"
#include "debug.h"
#include "str.h"

int String_new(String **s)
{
    check_ptr(s);
    *s = calloc(1, sizeof(**s));
    check_alloc(*s);

    return 0;
error:
    return -1;
}

static int __String_resize(String *s, size_t size)
{
    check_ptr(s);

    if (size < STRING_DEFAULT_SIZE) size = STRING_DEFAULT_SIZE;

    char *data;
    if (s->data) {
        data = realloc(s->data, size);
    } else {
        data = malloc(size);
    }
    check_alloc(data);
    s->data = data;
    s->mlen = size;
    if (s->slen > s->mlen) s->slen = s->mlen;

    return 0;
error:
    return -1;
}

void String_clear(String *s)
{
    s->slen = 0;
    if (s->data && s->mlen > STRING_DEFAULT_SIZE) {
        __String_resize(s, STRING_DEFAULT_SIZE);
    }
}

int String_set(String *s, const char *cstr, size_t len)
{
    check_ptr(s);
    check_ptr(cstr);
    size_t mlen = STRING_DEFAULT_SIZE;
    while (mlen < len + 1) mlen <<= 1;

    if (s->data == NULL) {
        s->data = malloc(mlen);
        check_alloc(s->data);
        s->mlen = mlen;
    } else if (s->mlen < mlen || s->mlen > (mlen << 2)) {
        check(!__String_resize(s, mlen), "Failed to expand string.");
    }

    memmove(s->data, cstr, len);
    s->data[len] = '\0';

    return 0;
error:
    return -1;
}

String *make_string(const char *cstr)
{
    check_ptr(cstr);
    String *s;
    check(!String_new(&s), "Failed to create new string.");
    check(!String_set(s, cstr, strnlen(cstr, 1024)), "Failed to assign cstr.");
    return s;
error:
    return NULL;
}

int String_compare(const String *s1, const String *s2)
{
    check_ptr(s1);
    check_ptr(s2);
    check(s1->data && s2->data, "No string data allocated.");
    return strncmp(s1->data, s2->data, __max(s1->slen, s2->slen));
error:
    return -2;
}
