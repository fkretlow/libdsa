#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static inline int snprintf_styled(char *buf, size_t size,
                                  const char *style, const char *fmt, ...)
{
    int rc = 0;
    int len = 0;

    va_list ap;
    va_start(ap, fmt);

    if (__ANSI_styles) {
        rc = snprintf(buf, size, "%s", style);
        if (rc < 0) { fprintf(stderr, "fail"); exit(-1); }
        size -= (size_t)rc; len += rc; buf += rc;
    }

    rc = vsnprintf(buf, size, fmt, ap);
    if (rc < 0) { fprintf(stderr, "fail"); exit(-1); }
    size -= (size_t)rc; len += rc; buf += rc;

    if (__ANSI_styles) {
        rc = snprintf(buf, size, "%s", ANSI_RESET);
        if (rc < 0) { fprintf(stderr, "fail"); exit(-1); }
        size -= (size_t)rc; len += rc; buf += rc;
    }

    va_end(ap);
    return len;
}

char _log_buf[256];
void _log(const char *file, const int line, const char *function,
          int label, const char *fmt, ...)
{
    char *buf = _log_buf;
    int rc = 0;
    size_t size = 256;

    rc = snprintf_styled(buf, size, STYLE_LOC, "%s:%d: ", file, line);
    size -= (size_t)rc; buf += rc;
    rc = snprintf_styled(buf, size, STYLE_FN, "%s: ", function);
    size -= (size_t)rc; buf += rc;

    const char *label_style;
    const char *label_text;
    switch (label) {
        case CALL:
            label_style = STYLE_CALL;
            label_text = "called with ";
            break;
        case ERROR:
            label_style = STYLE_ERROR;
            label_text = "error: ";
            break;
        case FAIL:
            label_style = STYLE_FAIL;
            label_text = "fail: ";
            break;
        case WARN:
            label_style = STYLE_WARN;
            label_text = "warning: ";
            break;
        case INFO:
            label_style = STYLE_INFO;
            label_text = "info: ";
            break;
        case DEBUG:
            label_style = STYLE_DEBUG;
            label_text = "";
            break;
        default:
            label_style = label_text = "";
    }

    rc = snprintf_styled(buf, size, label_style, "%s", label_text);
    size -= (size_t)rc; buf += rc;

    va_list ap;
    va_start(ap, fmt);
    rc = vsnprintf(buf, size, fmt, ap);
    va_end(ap);

    fprintf(LOG_FILE, "%s\n", _log_buf);
}
