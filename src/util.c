/*************************************************************************************************
 *
 * util.c
 * Implementation file for util.h.
 *
 ************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include "check.h"
#include "util.h"

/* Perform f nruns times and measure the average, maximum and minimum time it takes. The results
 * will be stored in S. */
void measure(void (*f)(void), stats *S, unsigned nruns, double warm_up_time)
{
    clock_t start, end;
    double duration;
    stats_initialize(S);

    /* warm up */
    start = clock();
    while ((double)(clock() - start) / CLOCKS_PER_SEC < warm_up_time) f();

    /* measure nruns runs */
    for (unsigned i = 0; i < nruns; ++i) {
        start = clock();
        f();
        end = clock();
        duration = (double)(end - start) / CLOCKS_PER_SEC;
        stats_add(S, duration);
    }
}


/* A very simple finite state machine that consumes an input stream word by word, skipping
 * characters for which the test function returns 0. After each sequence of accepted characters
 * the given process function is called on the buffer, then the buffer is reset. It's possible
 * to transform accepted characters with the optional transform function. */
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
