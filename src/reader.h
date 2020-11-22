/* A very simple finite state machine that consumes an input stream word by word, skipping
 * non-accepted characters. After each sequence of accepted characters the given process function
 * is called on the buffer, then the buffer is cleared. It's possible to transform accepted
 * characters with the optional transform function. */

#ifndef _reader_h
#define _reader_h

#include <stdlib.h>

int process_words(FILE *stream,
                  size_t blen,
                  int (*test) (int c),
                  char (*transform) (char c),
                  int (*process) (char *buf, size_t n, void *p),
                  void *p);

#endif // _reader_h
