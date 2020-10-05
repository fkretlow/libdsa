#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "quicksort.h"

#define TEST_SIZE 10000
#define MAX_INT 1000

#define ctos(t) (double)(t) / CLOCKS_PER_SEC

int compint(const void* a, const void* b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

int main()
{
    clock_t start, end;
    double duration, reference;
    int percent;

    srand((unsigned)time(NULL));

    int sample1[TEST_SIZE] = { 0 };
    int sample2[TEST_SIZE] = { 0 };
    for (size_t i = 0; i < TEST_SIZE; ++i) {
        sample1[i] = sample2[i] = rand() % MAX_INT;
    }

    start = clock();
    qsort(sample1, TEST_SIZE, sizeof(int), compint);
    end = clock();
    reference = ctos(end - start);
    debug("%-10s %lf  %3d%%", "qsort", reference, 100);

    start = clock();
    quicksort(sample2, TEST_SIZE, sizeof(int), compint);
    end = clock();
    duration = ctos(end - start);
    percent = (int)(duration / reference * 100.0);
    debug("%-10s %lf  %3d%%", "quicksort", duration, percent);

    check(is_sorted(sample2, TEST_SIZE, sizeof(int), compint), "Not sorted!");

    return 0;
error:
    return -1;
}
