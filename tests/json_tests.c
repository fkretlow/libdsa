#include <stdio.h>

#include "log.h"
#include "list.h"
#include "json.h"
#include "str.h"
#include "test.h"
#include "test_utils.h"
#include "type_interface.h"

int test_serialize_list_of_ints(void)
{
    list *L = list_new(&int_type);
    for (int i = 0; i < 8; ++i) {
        list_push_back(L, &i);
    }

    str *expected = str_from_cstr("[0, 1, 2, 3, 4, 5, 6, 7]");
    str *json = list_to_json(L, serialize_int);
    test(json != NULL);
    test(str_compare(expected, json) == 0);

    list_delete(L);
    str_delete(expected);
    str_delete(json);
    return 0;
}

int test_serialize_list_of_strings(void)
{
    list *L = list_new(&str_type);

    str *s = str_new();
    char *composers[3] = { "Haydn", "Mozart", "Beethoven" };
    for (int i = 0; i < 3; ++i) {
        str_assign_cstr(s, composers[i]);
        list_push_back(L, s);
    }

    str *expected = str_from_cstr("[\"Haydn\", \"Mozart\", \"Beethoven\"]");

    str *json = list_to_json(L, serialize_string);
    test(json != NULL);
    test(str_compare(json, expected) == 0);

    list_delete(L);
    str_delete(s);
    str_delete(json);
    str_delete(expected);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_serialize_list_of_ints);
    run_test(test_serialize_list_of_strings);
    test_suite_end();
}
