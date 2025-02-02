/*************************************************************************************************
 *
 * test.h
 *
 * Minimal testing framework. Makes use of the logging framework defined in ./src/log.h.
 * Initialize tests in main by test_suite_start(), run single tests (signature must be int
 * f(void), returning 0) with run_test(f), inside the tests use test(predicate) to assert
 * conditions. Wrap everything up with test_suite_end().
 *
 * Author: Florian Kretlow, 2020
 * License: MIT License
 *
 ************************************************************************************************/

#ifndef _test_h
#define _test_h

#include <stdio.h>
#include <string.h>
#include "log.h"

#define KRED "\x1B[31m"
#define KRESET "\x1B[0m"

static int _rc = -1;
static int _has_failed;

extern struct log_file log_files[MAX_LOG_FILES];

#define test_suite_start() \
        fprintf(stderr, "%s\n", __FILE__); \
        for (size_t i = 0; i < strlen(__FILE__); ++i) fputc('-', stderr); \
        fputc('\n', stderr); \
        log_files[0].stream=stderr; \
        log_files[0].use_ansi_styles=1; \
        log_files[0].suppress_debug_messages=0; \
        log_files[0].suppress_call_logs=1; \
        log_files[0].suppress_errors=0; \
        log_files[1].stream=fopen("./tests/tests.log", "a"); \
        log_files[1].use_ansi_styles=0; \
        log_files[1].suppress_debug_messages=0; \
        log_files[1].suppress_call_logs=0; \
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
        return -1; \
    } \
    log_files[0].suppress_errors = 0;

#endif /* _test_h */
