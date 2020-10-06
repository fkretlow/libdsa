#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "sort.h"
#include "stats.h"

#define N_ELEMENTS 10000
#define MAX_VALUE 1000
#define N_RUNS 1000

#define init_comparisons() \
    clock_t start, end; \
    double duration; \
    Stats stats; \
    printf("%-12s  %10s  %10s  %10s\n", "algorithm", "average", "min", "max"); \
    printf("------------  ----------  ----------  ----------\n");

#define measure(nruns, f, A, ...) \
    Stats_init(&stats); \
    start = clock(); \
    while ((double)(clock() - start) / CLOCKS_PER_SEC < 1.0) { \
        make_random((A), N_ELEMENTS, MAX_VALUE); \
        f((A), ##__VA_ARGS__); \
    } \
    for (size_t i = 0; i < nruns; ++i) { \
        make_random((A), N_ELEMENTS, MAX_VALUE); \
        start = clock(); \
        f((A), ##__VA_ARGS__); \
        end = clock(); \
        duration = (double)(end - start) / CLOCKS_PER_SEC; \
        Stats_add(&stats, duration); \
    } \
    printf("%-12s  %10f  %10f  %10f\n", #f, stats.avg, stats.min, stats.max);


static inline void make_random(int* A, size_t nmemb, unsigned maxv)
{
    for (size_t i = 0; i < nmemb; ++i) {
        A[i] = rand() % maxv;
    }
}

static inline int compint(const void* a, const void* b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

int main()
{
    init_comparisons();

    int* A = malloc(N_ELEMENTS * sizeof(*A));
    check_alloc(A);

    measure(N_RUNS, qsort, A, N_ELEMENTS, sizeof(*A), compint);
    measure(N_RUNS, quicksort, A, N_ELEMENTS, sizeof(*A), compint);
    measure(N_RUNS, mergesort, A, N_ELEMENTS, sizeof(*A), compint);

    free(A);
    return 0;
error:
    if (A) free(A);
    return -1;
}
