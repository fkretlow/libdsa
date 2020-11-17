#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

struct log_file log_files[MAX_LOG_FILES] = { 0 };

#define set_style(stream, style, use_styles) \
    if (use_styles) fprintf(stream, "%s%s", ANSI_RESET, style)

#define clear_style(stream, use_styles) \
    if (use_styles) fprintf(stream, "%s", ANSI_RESET)

void _log(const char *src_file, const int line, const char *function,
          int log_type, const char *fmt, ...)
{
    struct log_file file;
    for (int i = 0; i < MAX_LOG_FILES; ++i) {
        file = log_files[i];
        if (file.stream == NULL) break;
        if (file.suppress_errors && log_type == ERROR) continue;
        if (file.suppress_debug_messages && (log_type == DEBUG || log_type == CALL))
            continue;

        set_style(file.stream, STYLE_LOC, file.use_ansi_styles);
        fprintf(file.stream, "%s:%d: ", src_file, line);

        set_style(file.stream, STYLE_FN, file.use_ansi_styles);
        fprintf(file.stream, "%s: ", function);

        const char *label_style;
        const char *label_text;
        switch (log_type) {
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

        set_style(file.stream, label_style, file.use_ansi_styles);
        fprintf(file.stream, "%s", label_text);
        clear_style(file.stream, file.use_ansi_styles);

        va_list ap;
        va_start(ap, fmt);
        vfprintf(file.stream, fmt, ap);
        fputc('\n', file.stream);
        va_end(ap);
    }
}
