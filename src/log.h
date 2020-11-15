#ifndef _log_h
#define _log_h

#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_FILE stderr

int _suppress_errors;

#define log_call(fmt, ...) \
    fprintf(LOG_FILE, "Call: %s:%d %s: " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_info(fmt, ...) \
    fprintf(LOG_FILE, "Info: %s:%d %s: " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_warning(fmt, ...) \
    fprintf(LOG_FILE, ANSI_COLOR_YELLOW "Warning: " ANSI_COLOR_RESET \
            "%s:%d %s: " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_error(fmt, ...) \
    if (_suppress_errors == 0) { \
        fprintf(LOG_FILE, ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET \
                "%s:%d %s: " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    }

#endif // _log_h
