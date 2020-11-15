#ifndef _log_h
#define _log_h

#include <stdio.h>

#define ANSI_BOLD       "\x1b[1m"
#define ANSI_FAINT      "\x1b[2m"
#define ANSI_ITALIC     "\x1b[3m"
#define ANSI_RED        "\x1b[38;5;88m"
#define ANSI_ORANGE     "\x1b[38;5;214m"
#define ANSI_BLUE       "\x1b[38;5;69m"
#define ANSI_RESET      "\x1b[0m"

#define ANSI_RED_BG     "\x1b[48;5;88m"
#define ANSI_REVERSE    "\x1b[7m"

#define INFO            ANSI_FAINT ANSI_ITALIC "info:" ANSI_RESET
#define CALL            ANSI_FAINT ANSI_ITALIC "called with" ANSI_RESET
#define ERROR           ANSI_RED_BG " ERROR " ANSI_RESET
#define WARNING         ANSI_ORANGE ANSI_ITALIC "warning:" ANSI_RESET

#define LOG_FILE stderr

int _suppress_errors;
char _log_buf[64];

#define _loc() { \
    snprintf(_log_buf, 64, "%s:%d", __FILE__, __LINE__); \
    fprintf(LOG_FILE, ANSI_FAINT "%32s  " ANSI_RESET, _log_buf); \
}

#define _fn() fprintf(LOG_FILE, \
                      ANSI_BOLD ANSI_BLUE "%-24s " ANSI_RESET " ", \
                      __func__)
#define _label(l) fprintf(LOG_FILE, "%s ", l)
#define _msg(fmt, ...) fprintf(LOG_FILE, fmt, ##__VA_ARGS__)
#define _nl() fprintf(LOG_FILE, "%c", '\n')

#define log_call(fmt, ...) \
    _loc(); _fn(); _label(CALL); _msg(fmt, ##__VA_ARGS__); _nl();

#define log_info(fmt, ...) \
    _loc(); _fn(); _label(INFO); _msg(fmt, ##__VA_ARGS__); _nl()

#define log_debug(fmt, ...) \
    _loc(); _fn(); _msg(fmt, ##__VA_ARGS__); _nl()

#define log_warn(fmt, ...) \
    _loc(); _fn(); _label(WARNING); _msg(fmt, ##__VA_ARGS__); _nl()

#define log_error(fmt, ...) \
    if (_suppress_errors == 0) { \
        _loc(); _fn(); \
        _label(ERROR); \
        _msg(fmt, ##__VA_ARGS__); _nl(); \
    }

#endif // _log_h
