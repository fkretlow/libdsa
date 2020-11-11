#include <stdio.h>

#include "debug.h"
#include "list.h"
#include "json.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

int test_serialize_list_of_ints(void)
{
    List *L = List_new(&int_type);
    for (int i = 0; i < 8; ++i) {
        List_push_back(L, &i);
    }

    String *expected = String_from_cstr("[0, 1, 2, 3, 4, 5, 6, 7]");
    String *json = List_to_json(L, serialize_int);
    test(json != NULL, "Failed to serialize list of ints.");
    test(String_compare(expected, json) == 0,
            "json is not what we expect: '%s'", String_data(json));

    List_delete(L);
    String_delete(expected);
    String_delete(json);
    return TEST_OK;
}

int test_serialize_list_of_strings(void)
{
    List *L = List_new(&String_type);

    String *s = String_new();
    char *composers[3] = { "Haydn", "Mozart", "Beethoven" };
    for (int i = 0; i < 3; ++i) {
        String_assign_cstr(s, composers[i]);
        List_push_back(L, s);
    }

    String *expected = String_from_cstr("[\"Haydn\", \"Mozart\", \"Beethoven\"]");

    String *json = List_to_json(L, serialize_string);
    test(json != NULL, "Failed to serialize list of strings.");
    test(String_compare(json, expected) == 0, "expected != json = '%s'",
            String_data(json));

    List_delete(L);
    String_delete(s);
    String_delete(json);
    String_delete(expected);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_serialize_list_of_ints);
    run_test(test_serialize_list_of_strings);
    test_suite_end();
}
