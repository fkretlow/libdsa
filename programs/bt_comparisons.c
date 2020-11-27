#include <stdlib.h>
#include <time.h>

#include "binary_tree.h"
#include "stats.h"
#include "type_interface.h"
#include "util.h"

#define NRUNS 32
#define NGETS 20000
#define NMEMB 512
#define MAXV 4096

void bto(void)
{
    bt *T = bt_new(NONE, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        bt_insert(T, &i);
    }

    for (int i = 0; i < NGETS; ++i) {
        v = rand() % MAXV;
        bt_has(T, &v);
    }

    for (int i = 0; i < NMEMB; ++i) {
        bt_remove(T, &i);
    }
    bt_delete(T);
}

void rbto(void)
{
    bt *T = bt_new(RED_BLACK, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        bt_insert(T, &i);
    }

    for (int i = 0; i < NGETS; ++i) {
        v = rand() % MAXV;
        bt_has(T, &v);
    }

    for (int i = 0; i < NMEMB; ++i) {
        bt_remove(T, &i);
    }
    bt_delete(T);
}

void btr(void)
{
    bt *T = bt_new(NONE, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bt_insert(T, &v);
    }
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bt_remove(T, &v);
    }
    bt_delete(T);
}

void rbtr(void)
{
    bt *T = bt_new(RED_BLACK, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bt_insert(T, &v);
    }
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bt_remove(T, &v);
    }
    bt_delete(T);
}

int main(int argc, char *argv[])
{
    printf("We're up and running.\n");
    stats s_bto, s_btr, s_rbto, s_rbtr;

    measure(bto, &s_bto, NRUNS, 1.0);
    measure(btr, &s_btr, NRUNS, 1.0);
    measure(bto, &s_rbto, NRUNS, 1.0);
    measure(bto, &s_rbtr, NRUNS, 1.0);

    printf("%-15s  %10s  %10s  %10s\n", "test case", "avg", "min", "max");
    printf("---------------  ----------  ----------  ----------\n");
    printf("%-15s  %10f  %10f  %10f\n", "bt ordered",  s_bto.avg,  s_bto.min,  s_bto.max);
    printf("%-15s  %10f  %10f  %10f\n", "rbt ordered", s_rbto.avg, s_rbto.min, s_rbto.max);
    printf("%-15s  %10f  %10f  %10f\n", "bt random",   s_btr.avg,  s_btr.min,  s_btr.max);
    printf("%-15s  %10f  %10f  %10f\n", "rbt random",  s_rbtr.avg, s_rbtr.min, s_rbtr.max);

    return 0;
}
