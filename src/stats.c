#include <float.h>
#include "stats.h"

void stats_init(stats *s)
{
    s->avg = 0.0;
    s->min = DBL_MAX;
    s->max = DBL_MIN;
    s->count = 0;
}

void stats_add(stats *s, double v)
{
    s->avg = (s->count * s->avg + v) / (s->count + 1);
    ++s->count;
    if (v < s->min) {
        s->min = v;
    } else if (v > s->max) {
        s->max = v;
    }
}
