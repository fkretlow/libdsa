#include "check.h"
#include "list.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static list *L;
static int rc;

int test_list_new(void)
{
    L = list_new(&int_type);
    test(L != NULL);
    test(L->first == NULL && L->last == NULL);
    test(L->data_type->size == sizeof(int));
    test(L->count == 0);
    return 0;
}

int test_list_usage(void)
{
    int val = 1;
    int out;
    int *ip;

    rc = list_push_back(L, &val);
    test(rc == 1);
    test(L->first && L->last && L->first == L->last);
    test(L->first->next == NULL && L->first->prev == NULL);
    test(L->count == 1);

    ip = list_last(L);
    test(*ip == val);

    rc = list_pop_back(L, &out);
    test(rc == 1);
    test(out == val);
    test(L->first == NULL);
    test(L->last == NULL);
    test(L->count == 0);

    for (int i = 0; i < 8; ++i) {
        rc = list_push_back(L, &i);
        test(rc == 1);
    }
    test(L->count == 8);

    for (int i = 7; i >= 0; --i) {
        rc = list_pop_back(L, &out);
        test(rc == 1);
        test(out == i);
    }
    test(L->count == 0);

    for (int i = 0; i < 8; ++i) {
        rc = list_push_front(L, &i);
        test(rc == 1);
    }
    test(L->count == 8);

    val = 7;
    list_foreach(L, ip) {
        test(*ip == val);
        --val;
    }

    val = -1;
    rc = list_insert(L, 3, &val);
    test(rc == 1);
    test(L->count == 9);

    ip = list_get(L, 3);
    test(ip != NULL);
    test(*ip == -1);

    rc = list_remove(L, 3);
    test(rc == 1); /* TODO: Shouldn't this return 1 for a successful deletion? */
    test(L->count == 8);

    for (int i = 7; i >= 0; --i) {
        rc = list_pop_front(L, &out);
        test(rc == 1);
        test(out == i);
    }
    test(L->count == 0);

    rc = list_pop_back(L, NULL);
    test(rc == 0);

    return 0;
}

int test_list_teardown(void)
{
    list_clear(L);
    test(L->count == 0);
    list_delete(L);
    return 0;
}

int test_list_of_strings(void)
{
    int rc;

    L = list_new(&str_type);
    test(L != NULL);

    str *s = str_new();
    str out;

    str_assign_cstr(s, "Haydn");
    list_push_back(L, s);

    str_assign_cstr(s, "Mozart");
    list_push_back(L, s);

    str_assign_cstr(s, "Beethoven");
    list_push_back(L, s);

    rc = list_pop_back(L, &out);
    test(str_compare(s, &out) == 0);

    while (list_count(L) > 0){
        rc = list_pop_back(L, NULL);
        test(rc == 1);
    }

    rc = list_pop_back(L, NULL);
    test(rc == 0);

    list_delete(L);
    str_destroy(&out);
    str_delete(s);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_list_new);
    run_test(test_list_usage);
    run_test(test_list_teardown);
    run_test(test_list_of_strings);
    test_suite_end();
}
