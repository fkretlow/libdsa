#ifndef _util_h
#define _util_h

#include "stats.h"

void measure(void (*f)(void), stats *s, unsigned nruns, double warm_up_time);

#endif /* _util_h */
