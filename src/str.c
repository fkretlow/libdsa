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

void String_delete(String *s)
{
    if (s->data) free(s->data);
    free(s);
}

static int String_resize(String *s, size_t size)
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
        String_resize(s, STRING_DEFAULT_SIZE);
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
        check(!String_resize(s, mlen), "Failed to expand string.");
    }

    memmove(s->data, cstr, len);
    s->data[len] = '\0';
    s->slen = len;

    return 0;
error:
    return -1;
}

int String_copy(const String *src, String **copy_out)
{
    check_ptr(src);
    check_ptr(copy_out);
    check(src->data, "No String data allocated in source string.");

    check(!String_new(copy_out), "String_new failed.");
    check(!String_set(*copy_out, src->data, src->slen), "String_set failed.");

    return 0;
error:
    return -1;
}

String *make_string(const char *cstr)
{
    check_ptr(cstr);
    String *s;
    check(!String_new(&s), "Failed to create new string.");
    check(!String_set(s, cstr, strnlen(cstr, STRING_MAX_CSTR_LEN)),
            "Failed to assign cstr.");
    return s;
error:
    return NULL;
}

int String_compare(const String *s1, const String *s2)
{
    check_ptr(s1);
    check_ptr(s2);
    check(s1->data && s2->data, "No string data allocated.");
    return strncmp(s1->data, s2->data, _max(s1->slen, s2->slen));
error:
    return -2;
}

uint32_t String_hash(const String *s)
{
    return jenkins_hash(s->data, s->slen);
}

int String_append(String *s1, const String *s2)
{
    check_ptr(s1);
    check_ptr(s2);
    check(s1->data && s2->data, "No string data allocated.");

    size_t slen = s1->slen + s2->slen;
    size_t mlen = s1->mlen;
    while (mlen < slen + 1) mlen <<= 1;
    if (mlen > s1->mlen) {
        check(!String_resize(s1, mlen), "String_resize failed.");
    }

    memmove(s1->data + s1->slen, s2->data, s2->slen);
    s1->slen = slen;
    s1->data[slen] = '\0';

    return 0;
error:
    return -1;
}

int String_append_cstr(String *s1, const char *cstr)
{
    check_ptr(s1);
    check_ptr(cstr);
    check(s1->data, "No string data allocated.");

    size_t cstrlen = strnlen(cstr, STRING_DEFAULT_SIZE);
    size_t slen = s1->slen + cstrlen;
    size_t mlen = s1->mlen;
    while (mlen < slen + 1) mlen <<= 1;
    if (mlen > s1->mlen) {
        check(!String_resize(s1, mlen), "String_resize failed.");
    }

    memmove(s1->data + s1->slen, cstr, cstrlen);
    s1->slen = slen;
    s1->data[slen] = '\0';

    return 0;
error:
    return -1;
}

int String_concat(const String *s1, const String *s2, String **result_out)
{
    check_ptr(s1);
    check_ptr(s2);
    check_ptr(result_out);

    check(!String_copy(s1, result_out), "String_copy failed.");
    check(!String_append(*result_out, s2), "String_append failed.");

    return 0;
error:
    return -1;
}
