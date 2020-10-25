#include "debug.h"
#include "list.h"
#include "json.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"

static int rc;

int test_serialize_list(void)
{
    List L;
    List_init(&L, sizeof(int), NULL, NULL);
    for (int i = 0; i < 8; ++i) {
        List_push_back(&L, &i);
    }

    String expected = String_from_cstr("[0, 1, 2, 3, 4, 5, 6, 7]");
    String json = List_to_json(&L, serialize_int);
    test(json != NULL, "Failed to serialize list of ints.");
    test(String_compare(expected, json) == 0,
            "json is not what we expect: '%s'", json->data);

    List_clear(&L);
    String_delete(expected);
    String_delete(json);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_serialize_list);
    test_suite_end();
}
