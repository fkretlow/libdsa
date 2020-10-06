#ifndef _test_utils_h
#define _test_utils_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "debug.h"

int compint(const int* a, const int* b)
{
    return *a < *b ? -1 : *a > *b ? 1 : 0;
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
