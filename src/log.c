#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

FILE *log_files[MAX_LOG_FILES] = { NULL };

#define set_style(stream, style, use_styles) \
    if (use_styles) fprintf(stream, "%s%s", ANSI_RESET, style)

#define clear_style(stream, use_styles) \
    if (use_styles) fprintf(stream, "%s", ANSI_RESET)

char _log_buf[256];
void _log(const char *file, const int line, const char *function,
          int label, const char *fmt, ...)
{
    int use_styles;
    FILE *stream;
    for (int i = 0; i < MAX_LOG_FILES; ++i) {
        stream = log_files[i];
        if (stream == NULL) break;

        use_styles = (stream == stdout || stream == stderr) ? __ANSI_styles : 0;

        set_style(stream, STYLE_LOC, use_styles);
        fprintf(stream, "%s:%d: ", file, line);

        set_style(stream, STYLE_FN, use_styles);
        fprintf(stream, "%s: ", function);

        const char *label_style;
        const char *label_text;
        switch (label) {
            case INFO:
                label_style = STYLE_INFO;
                label_text = "info: ";
                break;
            case DEBUG:
                label_style = STYLE_DEBUG;
                label_text = "";
                break;
            case CALL:
                label_style = STYLE_CALL;
                label_text = "called with ";
                break;
            case WARN:
                label_style = STYLE_WARN;
                label_text = "warning: ";
                break;
            case ERROR:
                label_style = STYLE_ERROR;
                label_text = "error: ";
                break;
            case FAIL:
                label_style = STYLE_FAIL;
                label_text = "fail: ";
                break;
            case PASS:
                label_style = STYLE_PASS;
                label_text = "pass: ";
                break;
            default:
                label_style = label_text = "";
        }

        set_style(stream, label_style, use_styles);
        fprintf(stream, "%s", label_text);
        clear_style(stream, use_styles);

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stream, fmt, ap);
        fputc('\n', stream);
        va_end(ap);
    }
}
