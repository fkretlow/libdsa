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
    List_init(&L, sizeof(int), NULL);
    for (int i = 0; i < 8; ++i) {
        List_push_back(&L, &i);
    }

    String *json;
    rc = List_to_json(&L, serialize_int, &json);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    debug("%s", json->data);

    List_clear(&L);
    String_delete(json);
    List_init(&L, sizeof(String), NULL);

    String *s1 = make_string("waltz");
    String *s2 = make_string("mazurka");
    List_push_back(&L, s1);
    List_push_back(&L, s2);

    /* TODO: We need copy callbacks for the containers after all. Adding
    Strings (or any nested object with stuff on the heap) to a list doesn't
    work.*/

    rc = List_to_json(&L, serialize_string, &json);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    debug("%s", json->data);

    List_clear(&L);
    String_delete(s1);
    String_delete(s2);
    String_delete(json);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_serialize_list);
    test_suite_end();
}
