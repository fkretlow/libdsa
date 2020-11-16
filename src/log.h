#ifndef _log_h
#define _log_h

#include <stdio.h>

#define MAX_LOG_FILES 8
FILE *log_files[MAX_LOG_FILES];

int _suppress_errors;
int __ANSI_styles;

#define ANSI_BOLD       "\x1b[1m"
#define ANSI_FAINT      "\x1b[2m"
#define ANSI_ITALIC     "\x1b[3m"
#define ANSI_RED        "\x1b[31m"
#define ANSI_ORANGE     "\x1b[38;5;214m"
#define ANSI_BLUE       "\x1b[38;5;69m"
#define ANSI_GREEN      "\x1b[38;5;35m"
#define ANSI_RESET      "\x1b[0m"

#define STYLE_LOC       ANSI_FAINT
#define STYLE_FN        ANSI_BLUE ANSI_BOLD
#define STYLE_INFO      ""
#define STYLE_DEBUG     ""
#define STYLE_CALL      ANSI_FAINT
#define STYLE_WARN      ANSI_ORANGE
#define STYLE_ERROR     ANSI_RED ANSI_BOLD
#define STYLE_FAIL      ANSI_RED ANSI_BOLD
#define STYLE_PASS      ANSI_GREEN ANSI_BOLD

enum _log_labels { DEBUG, INFO, CALL, WARN, ERROR, FAIL, PASS };
void _log(const char *file, const int line, const char *function,
          int label, const char *fmt, ...);

#ifndef NDEBUG
#define log_debug(fmt, ...) _log(__FILE__, __LINE__, __func__, DEBUG, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

#define log_info(fmt, ...)  _log(__FILE__, __LINE__, __func__, INFO,  fmt, ##__VA_ARGS__)
#define log_call(fmt, ...)  _log(__FILE__, __LINE__, __func__, CALL,  fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  _log(__FILE__, __LINE__, __func__, WARN,  fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) _log(__FILE__, __LINE__, __func__, ERROR, fmt, ##__VA_ARGS__)
#define log_fail(fmt, ...)  _log(__FILE__, __LINE__, __func__, FAIL,  fmt, ##__VA_ARGS__)
#define log_pass(fmt, ...)  _log(__FILE__, __LINE__, __func__, PASS,  fmt, ##__VA_ARGS__)

#endif // _log_h
