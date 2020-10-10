#include "debug.h"
#include "list.h"
#include "unittest.h"

static List L;
int rc;

int test_list_init(void)
{
    rc = List_init(&L, sizeof(int), NULL);
    test(rc == 0, "List_init failed.");
    test(L.first == NULL && L.last == NULL, "L.first != NULL or L.last != NULL");
    test(L.element_size == sizeof(int),
            "L.element_size = %lu (%lu)", L.element_size, sizeof(int));
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);
    test(L.destroy == NULL, "L.destroy = %p (%p)", L.destroy, NULL);
    return TEST_OK;
}

int test_list_push_pop(void)
{
    int val = 1;
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
        test(rc == 0, "List_push_back failed (loop iteration #%d)", i);
    }

    return TEST_OK;
}

int test_list_clear(void)
{
    List_clear(&L);
    test(L.size == 0, "L.size = %lu (%lu)", L.size, 0lu);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_list_init);
    run_test(test_list_push_pop);
    run_test(test_list_clear);
    test_suite_end();
}
