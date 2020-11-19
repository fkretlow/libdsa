#ifndef _str_h
#define _str_h

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

#define STR_INTERNAL_CAPACITY (sizeof(uint32_t) + sizeof(char*))
#define STR_MAX_CAPACITY UINT32_MAX

typedef struct str {
    uint32_t length;
    uint8_t data_external;
    union {
        struct {
            char data[STR_INTERNAL_CAPACITY];
        } internal;
        struct {
            uint32_t capacity;
            char *data;
        } external;
    } data;
} str;

#define str_length(S) ((S)->length)
#define str_empty(S) ((S)->slen == 0)
#define str_data(s) ( (s)->data_external \
                            ? (s)->data.external.data \
                            : (char*)(s)->data.internal.data )
#define str_capacity(s) ( (s)->data_external \
                                ? (s)->data.external.capacity \
                                : STR_INTERNAL_CAPACITY )

int         str_initialize      (str *s);
str *       str_new             (void);
void        str_destroy         (void *s);
void        str_delete          (str *s);

int         str_reserve         (str *s, const size_t capacity);
int         str_shrink_to_fit   (str *s);

void        str_clear           (str *s);

str *       str_copy            (const str *src);
void        str_copy_to         (void *dest, const void *src);

int         str_assign          (str *dest, const str *src);
int         str_assign_cstr     (str *dest, const char *cstr);
str *       str_from_cstr       (const char *cstr);
int         str_append          (str *s1, const str *s2);
int         str_append_cstr     (str *s, const char *cstr);
int         str_push_back       (str *s, const char c);
int         str_pop_back        (str *s, char *out);
str *       str_concat          (const str *s1, const str *s2);

int         str_compare         (const void *a, const void *b);
uint32_t    str_hash            (const void *s);
void        str_print           (FILE *stream, const void *s);

#endif // _str_h
