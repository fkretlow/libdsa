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

extern int __suppress_errors;

#define test_suite_start() printf("\n%s\n" \
        "-------------------------------------------------\n", \
        __FILE__); \
        __suppress_errors = 0;

#define test_suite_end() if (__tests_failed == 0) { \
    return 0; \
} else { \
    return 1; \
}

#define run_test(T) fprintf(stderr, #T "\n"); \
    __rc = (T)(); \
    if (__rc != TEST_ERR) { \
        __tests_ok++; \
    } else { \
        __tests_failed++; \
    } \
    __tests_run++;

#define test(T, M, ...) if (!(T)) { \
    fprintf(stderr, KRED "   Fail: " KRESET M " [%d]\n", \
            ##__VA_ARGS__, __LINE__); \
    return TEST_ERR; \
}

#define test_fail(T, M, ...) __suppress_errors = 1; \
    if (!(T)) { \
        fprintf(stderr, KRED "   Fail: " KRESET M " [%d]\n", \
                ##__VA_ARGS__, __LINE__); \
        __suppress_errors = 0; \
        return TEST_ERR; \
    } \
    __suppress_errors = 0;

#endif // _unittest_h
