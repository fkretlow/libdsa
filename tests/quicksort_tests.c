#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "sort.h"
#include "test_utils.h"
#include "unittest.h"

#define N_ELEMENTS 100
#define MAX_VALUE 100

static int A[N_ELEMENTS];

int test_quicksort(void)
{
    make_random(A, N_ELEMENTS, MAX_VALUE);
    quicksort(A, N_ELEMENTS, sizeof(*A), compint);
    test(is_sorted(A, N_ELEMENTS, sizeof(*A), compint), "Not sorted.")
    return TEST_OK;
}

int main()
{
    test_suite_start();
    run_test(test_quicksort);
    test_suite_end();
}
