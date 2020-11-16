/* unittest.h
 * Macros for building simple tests.
 *
 * run_test(test_name) run the test function
 * test_suite_start() initiate test suite
 * test_suite_end() finish test suite */

#ifndef _unittest_h
#define _unittest_h

#include <stdio.h>
#include <string.h>
#include "log.h"

#define TEST_OK 0
#define TEST_ERROR -1

#define KRED "\x1B[31m"
#define KRESET "\x1B[0m"

static int _tests_run = 0;
static int _tests_ok = 0;
static int _tests_failed = 0;
static int _rc = TEST_ERROR;

extern int _suppress_errors;
extern int __ANSI_styles;

#define test_suite_start() printf("\n%s\n" \
        "-------------------------------------------------\n", \
        __FILE__); \
        _suppress_errors = 0; \
        __ANSI_styles = 1

#define test_suite_end() printf("\n"); if (_tests_failed == 0) { \
    return 0; \
} else { \
    return -1; \
}

#define run_test(T) { \
    _rc = T(); \
    if (_rc != 0) log_fail("%s", #T); \
}

#define test(T, fmt, ...) if (!(T)) { \
    log_error(fmt, ##__VA_ARGS__); \
    return -1; \
}

#define test_fail(T, fmt, ...) _suppress_errors = 1; \
    if (!(T)) { \
        log_fail(fmt, ##__VA_ARGS__); \
        return TEST_ERROR; \
    } \
    _suppress_errors = 0;

#endif // _unittest_h
