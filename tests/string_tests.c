#include "debug.h"
#include "test.h"
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
    String_delete(s3);
    return TEST_OK;
}

int test_string_copy(void)
{
    String *s1 = make_string("Chopin");
    String *s2;

    rc = String_copy(s1, &s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s2->data, "Chopin", 7) == 0, "s2->data = '%s'", s1->data);

    String_delete(s1);
    String_delete(s2);

    return TEST_OK;
}

int test_string_append(void)
{
    String *s1 = make_string("spicey");
    String *s2 = make_string(" bolognese");

    rc = String_append(s1, s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s1->data, "spicey bolognese", 17) == 0, "s1->data = '%s'", s1->data);
    test(s1->slen == 16, "s1->slen = %lu (%lu)", s1->slen, 16lu);
    test(s1->mlen == 32, "s1->mlen = %lu (%lu)", s1->mlen, 32lu);

    rc = String_append_cstr(s1, " from Bologna");
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s1->data, "spicey bolognese from Bologna", 30) == 0,
            "s1->data = '%s'", s1->data);
    test(s1->slen == 29, "s1->slen = %lu (%lu)", s1->slen, 29lu);

    String_delete(s1);
    String_delete(s2);
}

int test_string_concat(void)
{
    String *s1 = make_string("abstract");
    String *s2 = make_string(" algebra");
    String *s3;

    rc = String_concat(s1, s2, &s3);

    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s3->data, "abstract algebra", 17) == 0, "s1->data = '%s'", s1->data);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);

    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_make_string);
    run_test(test_string_compare);
    run_test(test_string_copy);
    run_test(test_string_append);
    run_test(test_string_concat);
    test_suite_end();
}
