/*************************************************************************************************
 *
 * stats.c
 * Implementation file for stats.h.
 *
 ************************************************************************************************/

#include <float.h>
#include "stats.h"

/* void stats_initialize(stats *S) -- self-explainging */
void stats_initialize(stats *S)
{
    S->avg = 0.0;
    S->min = DBL_MAX;
    S->max = DBL_MIN;
    S->count = 0;
}

/* void stats_add(stats *S, double v)
 * Have the rolling stats munch v and update themselves accordingly. */
void stats_add(stats *S, double v)
{
    S->avg = (S->count * S->avg + v) / (S->count + 1);
    ++S->count;
    if      (v < S->min)    S->min = v;
    else if (v > S->max)    S->max = v;
}
