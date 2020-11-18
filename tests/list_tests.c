#include "check.h"
#include "list.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

static List *L;
static int rc;

int test_list_new(void)
{
    L = List_new(&int_type);
    test(L != NULL);
    test(L->first == NULL && L->last == NULL);
    test(L->element_type->size == sizeof(int));
    test(L->size == 0);
    return 0;
}

int test_list_usage(void)
{
    int val = 1;
    int *ip;

    rc = List_push_back(L, &val);
    test(rc == 0);
    test(L->first && L->last && L->first == L->last);
    test(L->first->next == NULL && L->first->prev == NULL);
    test(L->size == 1);

    val = 0;
    rc = List_pop_back(L, &val);
    test(rc == 0);
    test(L->first == NULL);
    test(L->last == NULL);
    test(L->size == 0);
    test(val == 1);

    for (int i = 0; i < 8; ++i) {
        rc = List_push_back(L, &i);
        test(rc == 0);
    }
    test(L->size == 8);

    for (int i = 7; i >= 0; --i) {
        rc = List_pop_back(L, &val);
        test(rc == 0);
        test(val == i);
    }
    test(L->size == 0);

    for (int i = 0; i < 8; ++i) {
        rc = List_push_front(L, &i);
        test(rc == 0);
    }
    test(L->size == 8);

    val = 7;
    List_foreach(L, ip) {
        test(*ip == val);
        --val;
    }

    val = -1;
    rc = List_insert(L, 3, &val);
    test(rc == 0);
    test(L->size == 9);

    val = 0;
    rc = List_get(L, 3, &val);
    test(rc == 0);
    test(val == -1);

    rc = List_remove(L, 3);
    test(rc == 0);
    test(L->size == 8);

    for (int i = 7; i >= 0; --i) {
        rc = List_pop_front(L, &val);
        test(rc == 0);
        test(val == i);
    }
    test(L->size == 0);

    test_fail(List_pop_back(L, NULL) == -1, "List_pop_back on empty list didn't fail.");

    return 0;
}

int test_list_teardown(void)
{
    List_clear(L);
    test(L->size == 0);
    List_delete(L);
    return 0;
}

int test_list_of_strings(void)
{
    L = List_new(&str_type);
    test(L != NULL);

    str *s = str_new();
    str_assign_cstr(s, "Haydn");
    List_push_back(L, s);

    str_assign_cstr(s, "Mozart");
    List_push_back(L, s);

    str_assign_cstr(s, "Beethoven");
    List_push_back(L, s);

    str out;
    List_pop_back(L, &out);
    test(str_compare(s, &out) == 0);
    str_destroy(&out);

    List_delete(L);
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
