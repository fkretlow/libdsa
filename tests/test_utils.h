#ifndef _test_utils_h
#define _test_utils_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "debug.h"
#include "str.h"

int compint(const void *a, const void *b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

String serialize_int(const void *ip)
{
    int i = *(int *)ip;
    String s = String_new();
    check(s != NULL, "Failed to create String for serialized int.");
    size_t size = 1 * sizeof(char);
    if (i) size = (size_t)(floor(log10(i)) + 1) * sizeof(char);
    check(!String_reserve(s, size + 1), "Failed to reserve memory in String.");
    sprintf(s->data, "%d", i);
    s->data[size] = '\0';
    s->size = size;
    return s;
error:
    if (s) String_delete(s);
    return NULL;
}

String serialize_string(const void *s)
{
    String out = String_from_cstr("'");
    check(out != NULL, "Failed to create String for serialized string.");
    check(!String_append(out, *(String *)s), "Failed to append string to result string.");
    check(!String_push_back(out, '\''), "Failed to append trailing ' to result string.");
    return out;
error:
    if (out) String_delete(out);
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
