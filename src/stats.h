#ifndef _stats_h
#define _stats_h

#include <float.h>
#include <stdlib.h>

typedef struct stats {
    double avg;
    double min;
    double max;
    size_t count;
} stats;

void stats_init (stats *s);
void stats_add  (stats *s, double v);

#endif // _stats_h
