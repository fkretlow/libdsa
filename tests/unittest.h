/* unittest.h
 * Macros for building simple tests.
 *
 * make_test(test_name) create a test function (return TEST_OK)
 * run_test(test_name) run the test function
 * test_suite_start() initiate test suite
 * test_suite_end() finish test suite */

#ifndef _unittest_h
#define _unittest_h

#include <stdio.h>
#include <string.h>
#include <debug.h>

#define TEST_OK 0
#define TEST_ERR -1

#define KRED "\x1B[31m"
#define KRESET "\x1B[0m"

static int __tests_run = 0;
static int __tests_failed = 0;
static int __tests_ok = 0;
static int __rc = TEST_ERR;

#define test_suite_start() printf("\nRUNNING %s\n", __FILE__);
#define test_suite_end() if (!__tests_failed) { \
    printf("OK %d/%d\n", __tests_ok, __tests_run); \
    return 0; \
} else { \
    printf("%d/%d tests failed.\n", __tests_failed, __tests_run); \
    return 1; \
}

#define run_test(T) __rc = (T)(); \
    if (__rc != TEST_ERR) { \
        __tests_ok++; \
    } else { \
        __tests_failed++; \
    } \
    __tests_run++;

#define test(T, M, ...) if (!(T)) { \
    fprintf(stderr, KRED "FAIL: " KRESET M "\n", ##__VA_ARGS__); \
    return TEST_ERR; \
}

#endif // _unittest_h
