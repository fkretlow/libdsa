#include "check.h"
#include "reader.h"
#include <stdio.h>

int process_words(FILE *stream,
                  size_t blen,
                  int (*test) (int c),
                  char (*transform) (char c),
                  int (*process) (char *buf, size_t n, void *p),
                  void *p)
{
    check(stream != NULL, "no stream");
    check(blen > 0, "zero buffer length");
    check(test != NULL, "no test function");
    check(process != NULL, "no process function");

    int i = 0;
    char c = 0;
    char *buffer = malloc(blen);
    check_alloc(buffer);

    for ( ;; ) {
        c = fgetc(stream);
        if (test(c)) {
            if (transform) c = transform(c);
            buffer[i++] = c;
        } else {
            if (buffer[0] != '\0') {
                buffer[i] = '\0';
                process(buffer, i, p);
                buffer[i=0] = '\0';
            }

            if (c == EOF) break;
        }
    }

    free(buffer);
    return 0;
error:
    return -1;
}
