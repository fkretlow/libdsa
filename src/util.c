/*************************************************************************************************
 *
 * util.c
 *
 * Implementation file for util.h.
 *
 ************************************************************************************************/

#include <time.h>
#include "util.h"

/* void measure(void (*f)(void), stats *S, unsigned nruns, double warm_up_time)
 * Perform f nruns times and measure the average, maximum and minimum time it takes. The results
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
