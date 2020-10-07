#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "sort.h"
#include "test_utils.h"
#include "unittest.h"

#define N_ELEMENTS 1024
#define MAX_VALUE 100
#define N_RUNS 100

static int A[N_ELEMENTS];

int test_is_sorted(void)
{
    make_random(A, N_ELEMENTS, MAX_VALUE);
    test(!is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "is_sorted returned true for unsorted array.");
    make_equal(A, N_ELEMENTS, 0);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "is_sorted returned false for array with equal values.");
    make_sorted(A, N_ELEMENTS);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "is_sorted returned false for sorted array.");
    return TEST_OK;
}

int test_quicksort(void)
{
    for (int i = 0; i < N_RUNS; ++i) {
        make_random(A, N_ELEMENTS, MAX_VALUE);
        quicksort(A, N_ELEMENTS, sizeof(*A), compint);
        test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint), "Quicksort: Not sorted.");
    }
    quicksort(A, N_ELEMENTS, sizeof(*A), compint);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "Quicksort: Not sorted after sorting already sorted array.");
    return TEST_OK;
}

int test_mergesort(void)
{
    for (int i = 0; i < N_RUNS; ++i) {
        make_random(A, N_ELEMENTS, MAX_VALUE);
        mergesort(A, N_ELEMENTS, sizeof(*A), compint);
        test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint), "Mergesort: Not sorted.");
    }
    mergesort(A, N_ELEMENTS, sizeof(*A), compint);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "Mergesort: Not sorted after sorting already sorted array.");
    return TEST_OK;
}

int test_heapsort(void)
{
    for (int i = 0; i < N_RUNS; ++i) {
        make_random(A, N_ELEMENTS, MAX_VALUE);
        heapsort(A, N_ELEMENTS, sizeof(*A), compint);
        test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint), "Heapsort: Not sorted.");
    }
    heapsort(A, N_ELEMENTS, sizeof(*A), compint);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint),
            "Heapsort: Not sorted after sorting already sorted array.");
    return TEST_OK;
}

int main()
{
    srand((unsigned)time(NULL));
    test_suite_start();
    run_test(test_is_sorted);
    run_test(test_quicksort);
    run_test(test_mergesort);
    run_test(test_heapsort);
    test_suite_end();
}
