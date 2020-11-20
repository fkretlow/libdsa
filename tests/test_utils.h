#ifndef _test_utils_h
#define _test_utils_h

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "check.h"
#include "str.h"

str *random_str(uint32_t l)
{
    int rc;
    str *s = str_new();
    rc = str_reserve(s, l + 1);
    check_rc(rc, "str_reserve");

    for (unsigned i = 0; i < l; ++i) str_push_back(s, rand() % 26 + 97);
    return s;

error:
    return NULL;
}

int str_make_random(str *s, uint32_t l)
{
    int rc = str_reserve(s, l + 1);
    check_rc(rc, "str_reserve");
    s->length = 0; str_data(s)[0] = '\0';
    for (unsigned i = 0; i < l; ++i) str_push_back(s, rand() % 26 + 97);
    return 0;
error:
    return -1;
}

str *serialize_int(const void *ip)
{
    int i = *(int *)ip;
    str *s = str_new();
    check(s != NULL, "Failed to create str for serialized int.");
    size_t length = 1 * sizeof(char);
    if (i) length = (size_t)(floor(log10(i)) + 1) * sizeof(char);
    check(!str_reserve(s, length + 1), "Failed to reserve memory in str.");
    char *s_data = str_data(s);
    sprintf(s_data, "%d", i);
    s_data[length] = 0;
    s->length = length;
    return s;
error:
    if (s) str_delete(s);
    return NULL;
}

str *serialize_string(const void *s)
{
    str *out = str_from_cstr("\"");
    check(out != NULL, "Failed to create str for serialized string.");
    check(!str_append(out, (str *)s), "Failed to append string to result string.");
    check(!str_push_back(out, '"'), "Failed to append trailing ' to result string.");
    return out;
error:
    if (out) str_delete(out);
    return NULL;
}

void make_random(int* A, size_t n_members, unsigned max_value)
{
    for (size_t i = 0; i < n_members; ++i) A[i] = rand() % max_value;
}

void make_equal(int* A, size_t n_members, unsigned value)
{
    for (size_t i = 0; i < n_members; ++i) A[i] = value;
}


void make_sorted(int* A, size_t n_members)
{
    for (size_t i = 0; i < n_members; ++i) A[i] = i;
}

void print_array(int* A, size_t n_members)
{
    for (size_t i = 0; i < n_members; ++i) printf("%d ", A[i]);
    printf("\n");
}

#endif // _test_utils_h
