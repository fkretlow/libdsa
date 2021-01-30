/*************************************************************************************************
 *
 * util.h
 * Various utilities.
 *
 ************************************************************************************************/

#ifndef _util_h
#define _util_h

#include "stats.h"

/* Measure execution times. */
void measure(void (*f)(void), stats *s, unsigned nruns, double warm_up_time);

/* Read and process sequences of characters from a stream.*/
int process_words(FILE *stream,
                  size_t blen,
                  int (*test) (int c),
                  char (*transform) (char c),
                  int (*process) (char *buf, size_t n, void *p),
                  void *p); /* external parameter, is passed to the process callback */
#endif /* _util_h */
