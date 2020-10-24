#ifndef _str_h
#define _str_h

#include <stdint.h>
#include <stdlib.h>

#include "hash.h"

#define STRING_DEFAULT_SIZE 32
#define STRING_MAX_CSTR_LEN 1024

typedef struct String {
    char *data;
    size_t slen;
    size_t mlen;
} String;

#define String_size(S) ((S)->end)
#define String_empty(S) ((S)->end == 0)

int String_new(String **s);
void String_delete(String *s);
int String_set(String *s, const char *cstr, size_t len);
void String_clear(String *s);
int String_copy(const String *src, String **copy_out);

int String_compare(const String *s1, const String *s2);
uint32_t String_hash(const String *s);

int String_append(String *s1, const String *s2);
int String_append_cstr(String *s, const char *cstr);
int String_concat(const String *s1, const String *s2, String **result_out);

String *make_string(const char *cstr);

#endif // _str_h
