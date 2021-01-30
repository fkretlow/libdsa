/*************************************************************************************************
 *
 * sort_comparisons.c
 *
 * Compare the performance of the different sorting algorithms (including qsort from libc for
 * reference).
 *
 ************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "check.h"
#include "sort.h"
#include "stats.h"

#define NMEMB 10000
#define MAXV 1000
#define NRUNS 10

#define init_comparisons() \
    clock_t start, end; \
    double duration; \
    stats S; \
    printf("%-12s  %10s  %10s  %10s\n", "algorithm", "avg", "min", "max"); \
    printf("------------  ----------  ----------  ----------\n");

#define measure(nruns, f, A, ...) \
    stats_initialize(&S); \
    start = clock(); \
    while ((double)(clock() - start) / CLOCKS_PER_SEC < 1.0) { \
        make_random((A), NMEMB, MAXV); \
        f((A), ##__VA_ARGS__); \
    } \
    for (size_t i = 0; i < nruns; ++i) { \
        make_random((A), NMEMB, MAXV); \
        start = clock(); \
        f((A), ##__VA_ARGS__); \
        end = clock(); \
        duration = (double)(end - start) / CLOCKS_PER_SEC; \
        stats_add(&S, duration); \
    } \
    printf("%-12s  %10f  %10f  %10f\n", #f, S.avg, S.min, S.max);


static inline void make_random(int* A, size_t nmemb, unsigned maxv)
{
    for (size_t i = 0; i < nmemb; ++i) {
        A[i] = rand() % maxv;
    }
}

static inline int compint(const void *a, const void *b) { return *(int*)a - *(int*)b; }

int main()
{
    init_comparisons();

    int* A = malloc(NMEMB * sizeof(*A));
    check_alloc(A);

    measure(NRUNS, qsort,     A, NMEMB, sizeof(*A), compint);
    measure(NRUNS, quicksort, A, NMEMB, sizeof(*A), compint);
    measure(NRUNS, mergesort, A, NMEMB, sizeof(*A), compint);
    measure(NRUNS, heapsort,  A, NMEMB, sizeof(*A), compint);

    free(A);
    return 0;
error:
    if (A) free(A);
    return -1;
}
