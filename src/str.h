#ifndef _str_h
#define _str_h

#include <stdint.h>
#include <stdlib.h>

#include "hash.h"

#define STRING_DEFAULT_SIZE 32

typedef struct String {
    char *data;
    size_t slen;
    size_t mlen;
} String;

#define String_size(S) ((S)->end)
#define String_empty(S) ((S)->end == 0)

int String_new(String **s);
inline void String_delete(String *s);
int String_set(String *s, const char *cstr, size_t len);
void String_clear(String *s);
int String_compare(const String *s1, const String *s2);
inline uint32_t String_hash(const String *s);

String *make_string(const char *cstr);

inline void String_delete(String *s)
{
    if (s->data) free(s->data);
    free(s);
}

inline uint32_t String_hash(const String *s)
{
    return jenkins_hash(s->data, s->slen);
}

#endif // _str_h
