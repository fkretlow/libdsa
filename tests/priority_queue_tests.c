#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "priority_queue.h"
#include "test_utils.h"
#include "test.h"
#include "type_interface.h"

#define NMEMB 256
#define SLEN 63

int test_pqueue(void)
{
    int rc;
    str s1, s2;
    str_initialize(&s1);
    str_initialize(&s2);

    pqueue *Q = pqueue_new(&str_type);
    test(Q);

    for (int i = 0; i < NMEMB; ++i) {
        str_make_random(&s1, SLEN);
        rc = pqueue_enqueue(Q, &s1);
        test(rc == 1);
    }
    str_destroy(&s1);

    rc = pqueue_dequeue(Q, &s1);
    test(rc == 1);
    test(str_length(&s1) == 63);
    while (pqueue_count(Q) > 0) {
        rc = pqueue_dequeue(Q, &s2);
        test(rc == 1);
        test(str_compare(&s1, &s2) >= 0);
        str_clear(&s1);
        t_move(&str_type, &s1, &s2);
    }

    str_clear(&s2);
    rc = pqueue_dequeue(Q, &s2);
    test(rc == 0);
    test(str_length(&s2) == 0);

    str_destroy(&s1);
    str_destroy(&s2);
    pqueue_delete(Q);
    return 0;
}

int main(void)
{
    srand(1);

    test_suite_start();
    run_test(test_pqueue);
    test_suite_end();
}
