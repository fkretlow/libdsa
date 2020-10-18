#include "debug.h"
#include "unittest.h"
#include "str.h"

static int rc;

int test_make_string(void)
{
    String *s = make_string("Hello World");
    test(s != NULL, "s = NULL");
    test(strncmp(s->data, "Hello World", 11) == 0, "wrong data");
    test(s->slen == 11, "s->slen = %lu (%lu)", s->slen, 11lu);
    test(s->data[11] == 0, "s->data[11] = %c (%c)", s->data[11], 0);
    String_delete(s);
    return TEST_OK;
}

int test_string_compare(void)
{
    String *s1 = make_string("foo");
    String *s2 = make_string("foo");
    String *s3 = make_string("bar");

    rc = String_compare(s1, s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    rc = String_compare(s1, s3);
    test(rc > 0, "rc = %d (%d)", rc, 1);

    String_delete(s1);
    String_delete(s2);
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_make_string);
    run_test(test_string_compare);
    test_suite_end();
}
