#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stats.h"

#define NMEMB 512
#define NRUNS 1024

typedef unsigned char string_buffer[128];
void random_write(string_buffer *buf)
{
    unsigned i;
    for (i = 0; i < sizeof(*buf) - 1; ++i) {
        (*buf)[i] = rand() % 26 + 97;
    }
    (*buf)[i] = '\0';
}

struct node;
typedef struct node {
    struct node *next;
    void *data;
} node;


node *node_append(node *n, string_buffer *buf)
{
    node *m = calloc(1, sizeof(*m));
    m->data = calloc(1, sizeof(*buf));
    memcpy(m->data, buf, sizeof(*buf));
    m->next = n;
    return m;
}

void node_delete(node *n)
{
    node *next;
    while (n) {
        next = n->next;
        free(n->data);
        free(n);
        n = next;
    }
}

struct node_header;
typedef struct node_header {
    struct node_header *next;
} node_header;

#define node_header_data(n) ((char*)(n) + sizeof(node_header))

node_header *node_header_append(node_header *n, string_buffer *buf)
{
    node_header *m = calloc(1, sizeof(*m) + sizeof(*buf));
    memcpy(node_header_data(m), buf, sizeof(*buf));
    m->next = n;
    return m;
}

void node_header_delete(node_header *n)
{
    node_header *next;
    while (n) {
        next = n->next;
        free(n);
        n = next;
    }
}

void measure(void (*f)(void), Stats *stats, unsigned nruns, double warm_up_time)
{
    clock_t start, end;
    double duration;
    Stats_init(stats);

    /* warm up */
    while ((double)(clock() - start) / CLOCKS_PER_SEC < warm_up_time) f();

    /* measure nruns runs */
    for (unsigned i = 0; i < nruns; ++i) {
        start = clock();
        f();
        end = clock();
        duration = (double)(end - start) / CLOCKS_PER_SEC;
        Stats_add(stats, duration);
    }
}

void first_strategy(void) {
    string_buffer buf;
    int res;
    node *n = NULL;
    for (int i = 0; i < NMEMB; ++i) {
        random_write(&buf);
        n = node_append(n, &buf);
    }
    for (node *m = n; m != NULL; m = m->next) {
        res = ((const char *)m->data)[0] == 'a';
    }
    node_delete(n);
}

void second_strategy(void) {
    string_buffer buf;
    int res;
    node_header *n = NULL;
    for (int i = 0; i < NMEMB; ++i) {
        random_write(&buf);
        n = node_header_append(n, &buf);
    }
    for (node_header *m = n; m != NULL; m = m->next) {
        res = ((const char *)node_header_data(m))[0] == 'a';
    }
    node_header_delete(n);
}

int main(void)
{
    srand((unsigned)time(NULL));

    Stats s1, s2;
    measure(first_strategy,  &s1, NRUNS, 1.0);
    measure(second_strategy, &s2, NRUNS, 1.0);

    printf("%-10s  %10s  %10s  %10s\n", "strategy", "avg", "min", "max");
    printf("----------  ----------  ----------  ----------\n");
    printf("%-10s  %10f  %10f  %10f\n", "pointer", s1.avg, s1.min, s1.max);
    printf("%-10s  %10f  %10f  %10f\n", "aggregate", s2.avg, s2.min, s2.max);

    return 0;
}
