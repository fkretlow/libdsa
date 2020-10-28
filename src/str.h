#ifndef _str_h
#define _str_h

#include <stdint.h>
#include <stdlib.h>

#include "hash.h"

#define STRING_ALLOC_THRESHOLD (sizeof(size_t) + sizeof(char*))
#define STRING_MAX_SIZE 1024lu

typedef struct _string {
    size_t size;
    size_t capacity;
    char *data;
} _string;

typedef _string *String;

#define String_size(S) ((S)->slen)
#define String_empty(S) ((S)->slen == 0)

String String_new(void);
void String_delete(String s);
int String_reserve(String s, const size_t capacity);
int String_shrink_to_fit(String s);
void String_clear(String s);
String String_copy(const String src);
void String_copy_to(String *dest, const String *src);
int String_assign(String dest, const String src);
int String_assign_cstr(String dest, const char *cstr);
String String_from_cstr(const char *cstr);
int String_compare(const String s1, const String s2);
int String_append(String s1, const String s2);
int String_append_cstr(String s, const char *cstr);
int String_push_back(String s, const char c);
int String_pop_back(String s, char *out);
String String_concat(const String s1, const String s2);
unsigned long String_hash(const String *s);

#endif // _str_h
