#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "sort.h"
#include "test_utils.h"
#include "unittest.h"

#define N_ELEMENTS 1000
#define MAX_VALUE 1000
#define N_RUNS 1000

static int A[N_ELEMENTS];

int test_heapify(void)
{
    for (int i = 0; i < N_RUNS; ++i) {
        make_random(A, N_ELEMENTS, MAX_VALUE);
        __heapify(A, N_ELEMENTS);
        test(is_heap(A, N_ELEMENTS), "Not a heap after __heapify #%d.", i);
    }
    return TEST_OK;
}

int test_heapsort(void)
{
    int ok;
    for (int i = 0; i < N_RUNS; ++i) {
        make_random(A, N_ELEMENTS, MAX_VALUE);
        heapsort(A, N_ELEMENTS);
        ok = is_sorted(A, N_ELEMENTS, sizeof(*A), compint);
        if (!ok) print_array(A, N_ELEMENTS);
        test(ok, "not sorted after heapsort #%d", i);
    }
    return TEST_OK;
}

int main(void)
{
    /* srand((unsigned)time(NULL)); */
    srand(1);
    test_suite_start();
    run_test(test_heapify);
    run_test(test_heapsort);
    test_suite_end();
}
