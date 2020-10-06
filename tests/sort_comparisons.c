#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "quicksort.h"
#include "stats.h"

#define NMEMB 10000
#define MAXV 1000
#define NRUNS 1000

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
        make_random((A), NMEMB, MAXV); \
        f((A), ##__VA_ARGS__); \
    } \
    for (size_t i = 0; i < nruns; ++i) { \
        make_random((A), NMEMB, MAXV); \
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

    int* A = malloc(NMEMB * sizeof(*A));
    check_alloc(A);

    measure(100, qsort, A, NMEMB, sizeof(*A), compint);
    measure(100, quicksort, A, NMEMB, sizeof(*A), compint);

    free(A);
    return 0;
error:
    if (A) free(A);
    return -1;
}
