#ifndef _str_h
#define _str_h

#include "stdint.h"
#include "stdlib.h"

#define STRING_DEFAULT_SIZE 32

typedef struct String {
    char *data;
    size_t slen;
    size_t mlen;
} String;

#define String_size(S) ((S)->end)
#define String_empty(S) ((S)->end == 0)

int String_new(String **s);
void String_delete(String *s);
void String_clear(String *s);

int String_set(String *s, const char *cstr, size_t len);

int String_compare(const String *s1, const String *s2);
uint32_t String_hash(const String *s);

#endif // _str_h
