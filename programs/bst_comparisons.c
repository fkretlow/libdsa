#include <stdlib.h>
#include <time.h>

#include "bst.h"
#include "stats.h"
#include "type_interface.h"
#include "util.h"

#define NRUNS 32
#define NGETS 20000
#define NMEMB 512
#define MAXV 4096

void bst_ordered(void)
{
    bst *T = bst_new(NONE, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        bst_insert(T, &i);
    }

    for (int i = 0; i < NGETS; ++i) {
        v = rand() % MAXV;
        bst_has(T, &v);
    }

    for (int i = 0; i < NMEMB; ++i) {
        bst_remove(T, &i);
    }
    bst_delete(T);
}

void rb_ordered(void)
{
    bst *T = bst_new(RB, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        bst_insert(T, &i);
    }

    for (int i = 0; i < NGETS; ++i) {
        v = rand() % MAXV;
        bst_has(T, &v);
    }

    for (int i = 0; i < NMEMB; ++i) {
        bst_remove(T, &i);
    }
    bst_delete(T);
}

void bst_random(void)
{
    bst *T = bst_new(NONE, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bst_insert(T, &v);
    }
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bst_remove(T, &v);
    }
    bst_delete(T);
}

void rb_random(void)
{
    bst *T = bst_new(RB, &int_type, NULL);
    int v;
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bst_insert(T, &v);
    }
    for (int i = 0; i < NMEMB; ++i) {
        v = rand() % MAXV;
        bst_remove(T, &v);
    }
    bst_delete(T);
}

int main(void)
{
    stats s_bsto, s_bstr, s_rbo, s_rbr;

    measure(bst_ordered, &s_bsto, NRUNS, 1.0);
    measure(bst_random,  &s_bstr, NRUNS, 1.0);
    measure(rb_ordered,  &s_rbo,  NRUNS, 1.0);
    measure(rb_random,   &s_rbr,  NRUNS, 1.0);

    printf("%-15s  %10s  %10s  %10s\n", "test case", "avg", "min", "max");
    printf("---------------  ----------  ----------  ----------\n");
    printf("%-15s  %10f  %10f  %10f\n", "BST ordered",  s_bsto.avg, s_bsto.min, s_bsto.max);
    printf("%-15s  %10f  %10f  %10f\n", "RB  ordered",  s_rbo.avg,  s_rbo.min,  s_rbo.max);
    printf("%-15s  %10f  %10f  %10f\n", "BST random",   s_bstr.avg, s_bstr.min, s_bstr.max);
    printf("%-15s  %10f  %10f  %10f\n", "RB  random",   s_rbr.avg,  s_rbr.min,  s_rbr.max);

    return 0;
}
