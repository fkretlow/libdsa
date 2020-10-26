#include "debug.h"
#include "list.h"
#include "str.h"
#include "test.h"

static List L;
int rc;

int test_list_init(void)
{
    rc = List_init(&L, sizeof(int), NULL, NULL);
    test(rc == 0, "List_init failed.");
    test(L.first == NULL && L.last == NULL, "L.first != NULL or L.last != NULL");
    test(L.element_size == sizeof(int),
            "L.element_size = %lu (%lu)", L.element_size, sizeof(int));
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);
    test(L.destroy == NULL, "L.destroy = %p (%p)", L.destroy, NULL);
    return TEST_OK;
}

int test_list_usage(void)
{
    int val = 1;
    int *ip;

    rc = List_push_back(&L, &val);
    test(rc == 0, "List_push_back failed.");
    test(L.first && L.last && L.first == L.last,
            "Funny node connections after first push.");
    test(L.first->next == NULL && L.first->prev == NULL,
            "Single node has non-NULL connections.");
    test(L.size == 1, "L.size = %lu (%lu)", L.size, 1lu);

    val = 0;
    rc = List_pop_back(&L, &val);
    test(rc == 0, "List_pop_back failed.");
    test(L.first == NULL, "L.first = %p (%p)", L.first, NULL);
    test(L.last == NULL, "L.last = %p (%p)", L.last, NULL);
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);
    test(val == 1, "val = %d (%d)", val, 1);

    for (int i = 0; i < 8; ++i) {
        rc = List_push_back(&L, &i);
        test(rc == 0, "List_push_back failed (loop iteration i=%d)", i);
    }
    test(L.size == 8, "L.size = %lu (%lu)", L.size, 8lu);

    for (int i = 7; i >= 0; --i) {
        rc = List_pop_back(&L, &val);
        test(rc == 0, "List_pop_back failed (loop iteration i=%d)", i);
        test(val == i, "val = %d (%d)", val, i);
    }
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);

    for (int i = 0; i < 8; ++i) {
        rc = List_push_front(&L, &i);
        test(rc == 0, "List_push_back failed (loop iteration i=%d)", i);
    }
    test(L.size == 8, "L.size = %lu (%lu)", L.size, 8lu);

    val = 7;
    List_foreach(&L, ip) {
        test(*ip == val, "*ip = %d (%d)", *ip, val);
        --val;
    }

    val = -1;
    rc = List_insert(&L, 3, &val);
    test(rc == 0, "List_insert failed.");
    test(L.size == 9, "L.size = %lu (%lu)", L.size, 9lu);

    val = 0;
    rc = List_get(&L, 3, &val);
    test(rc == 0, "List_get failed.");
    test(val == -1, "val = %d (%d)", val, -1);

    rc = List_delete(&L, 3);
    test(rc == 0, "List_delete failed.");
    test(L.size == 8, "L.size = %lu (%lu)", L.size, 8lu);

    for (int i = 7; i >= 0; --i) {
        rc = List_pop_front(&L, &val);
        test(rc == 0, "List_pop_back failed (loop iteration i=%d)", i);
        test(val == i, "val = %d (%d)", val, i);
    }
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);

    test_fail(List_pop_back(&L, NULL) == -1, "List_pop_back on empty list didn't fail.");

    return TEST_OK;
}

int test_list_clear(void)
{
    List_clear(&L);
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);
    return TEST_OK;
}

int test_list_of_strings(void)
{
    rc = List_init(&L, sizeof(String), String_copy_to, String_delete);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    String s = String_new();
    String_assign_cstr(s, "Haydn");
    List_push_back(&L, &s);

    String_assign_cstr(s, "Mozart");
    List_push_back(&L, &s);

    String_assign_cstr(s, "Beethoven");
    List_push_back(&L, &s);

    String out;
    List_pop_back(&L, &out);
    test(String_compare(s, out) == 0, "s != out (out = \"%s\")", out->data);
    String_delete(out);

    List_clear(&L);
    String_delete(s);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_list_init);
    run_test(test_list_usage);
    run_test(test_list_clear);
    run_test(test_list_of_strings);
    test_suite_end();
}
