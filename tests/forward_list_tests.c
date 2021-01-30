#include "check.h"
#include "forward_list.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static flist *L;
static int rc;

int test_forward_list_new(void)
{
    L = flist_new(&int_type);
    test(L != NULL);
    test(L->front == NULL);
    test(L->data_type->size == sizeof(int));
    test(L->count == 0);
    return 0;
}

int test_forward_list_usage(void)
{
    int val = 1;
    int out;
    int *ip;

    rc = flist_push_front(L, &val);
    test(rc == 1);
    test(L->front != NULL);
    test(L->front->next == NULL);
    test(L->count == 1);

    ip = flist_front(L);
    test(*ip == val);

    rc = flist_pop_front(L, &out);
    test(rc == 1);
    test(out == val);
    test(L->front == NULL);
    test(L->count == 0);

    for (int i = 0; i < 8; ++i) {
        rc = flist_push_front(L, &i);
        test(rc == 1);
    }
    test(L->count == 8);

    for (int i = 7; i > 3; --i) {
        rc = flist_pop_front(L, &out);
        test(rc == 1);
        test(out == i);
    }
    test(L->count == 4);

    val = -1;
    rc = flist_insert(L, 2, &val);
    test(rc == 1);
    test(L->count == 5);

    ip = flist_get(L, 2);
    test(ip != NULL);
    test(*ip == -1);

    rc = flist_remove(L, 2);
    test(rc == 1);
    test(L->count == 4);

    for (int i = 3; i >= 0; --i) {
        rc = flist_pop_front(L, &out);
        test(rc == 1);
        test(out == i);
    }
    test(L->count == 0);

    rc = flist_pop_front(L, NULL);
    test(rc == 0);

    return 0;
}

int test_forward_list_teardown(void)
{
    flist_clear(L);
    test(L->count == 0);
    flist_delete(L);
    return 0;
}

int test_forward_list_of_strings(void)
{
    int rc;

    L = flist_new(&str_type);
    test(L != NULL);

    str *s = str_new();
    str out;

    str_assign_cstr(s, "Haydn");
    flist_push_front(L, s);

    str_assign_cstr(s, "Mozart");
    flist_push_front(L, s);

    str_assign_cstr(s, "Beethoven");
    flist_push_front(L, s);

    rc = flist_pop_front(L, &out);
    test(str_compare(s, &out) == 0);

    while (flist_count(L) > 0){
        rc = flist_pop_front(L, NULL);
        test(rc == 1);
    }

    rc = flist_pop_front(L, NULL);
    test(rc == 0);

    flist_delete(L);
    str_destroy(&out);
    str_delete(s);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_forward_list_new);
    run_test(test_forward_list_usage);
    run_test(test_forward_list_teardown);
    run_test(test_forward_list_of_strings);
    test_suite_end();
}

