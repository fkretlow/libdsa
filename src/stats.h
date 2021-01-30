/*************************************************************************************************
 *
 * stats.h
 *
 * Very simple rolling statistics, only keeps track of min, max and avg.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

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

void stats_initialize   (stats *S);
void stats_add          (stats *S, double v);

#endif // _stats_h
