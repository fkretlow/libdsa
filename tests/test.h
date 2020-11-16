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

static int _rc = TEST_ERROR;
static int _has_failed;

extern int _suppress_errors;
extern int __ANSI_styles;
extern FILE *log_files[8];

#define test_suite_start() \
        fprintf(stderr, "%s\n", __FILE__); \
        for (size_t i = 0; i < strlen(__FILE__); ++i) fputc('-', stderr); \
        fputc('\n', stderr); \
        _suppress_errors = 0; \
        __ANSI_styles = 1; \
        log_files[0] = stderr; \
        log_files[1] = fopen("./log.txt", "w"); \

#define test_suite_end() printf("\n"); \
    fflush(log_files[1]); \
    fclose(log_files[1]); \
    return 0

#define run_test(T) { \
    _rc = T(); \
    if (_rc != 0) { \
        log_fail("%s", #T); \
        _has_failed = 1; \
    } \
    else log_pass("%s", #T); \
}

#define test(T) if (!(T)) { \
    log_error("assertion `%s` failed", #T); \
    return -1; \
}

#define test_fail(T, fmt, ...) _suppress_errors = 1; \
    if (!(T)) { \
        log_fail(fmt, ##__VA_ARGS__); \
        return TEST_ERROR; \
    } \
    _suppress_errors = 0;

#endif // _unittest_h
