#ifndef _test_utils_h
#define _test_utils_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "check.h"
#include "str.h"

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
