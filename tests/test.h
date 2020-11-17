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

extern struct log_file log_files[MAX_LOG_FILES];

#define test_suite_start() \
        fprintf(stderr, "%s\n", __FILE__); \
        for (size_t i = 0; i < strlen(__FILE__); ++i) fputc('-', stderr); \
        fputc('\n', stderr); \
        log_files[0].stream=stderr; \
        log_files[0].use_ansi_styles=1; \
        log_files[0].suppress_debug_messages=1; \
        log_files[0].suppress_errors=0; \
        log_files[1].stream=fopen("log.txt", "a"); \
        log_files[1].use_ansi_styles=0; \
        log_files[1].suppress_debug_messages=0; \
        log_files[1].suppress_errors=0; \

#define test_suite_end() printf("\n"); \
    fflush(log_files[1].stream); \
    fclose(log_files[1].stream); \
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
    log_error("assertion `%s' failed", #T); \
    return -1; \
}

#define test_fail(T, fmt, ...) log_files[0].suppress_errors = 1; \
    if (!(T)) { \
        log_fail(fmt, ##__VA_ARGS__); \
        return TEST_ERROR; \
    } \
    log_files[0].suppress_errors = 0;

#endif // _unittest_h
