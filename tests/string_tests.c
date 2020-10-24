#include "debug.h"
#include "test.h"
#include "str.h"

static int rc;

int test_string_from_cstr(void)
{
    String s = String_from_cstr("Hello World");
    test(s != NULL, "s = NULL");
    test(strncmp(s->data, "Hello World", 11) == 0, "wrong data");
    test(s->size == 11, "s->size = %lu (%lu)", s->size, 11lu);
    test(s->data[11] == '\0', "s->data[11] = %c (%c)", s->data[11], '\0');
    String_delete(s);
    return TEST_OK;
}

int test_string_compare(void)
{
    String s1 = String_from_cstr("foo");
    String s2 = String_from_cstr("foo");
    String s3 = String_from_cstr("bar");

    rc = String_compare(s1, s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);

    rc = String_compare(s1, s3);
    test(rc > 0, "rc = %d (%d)", rc, 1);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);
    return TEST_OK;
}

int test_string_assign(void)
{
    String s1 = String_from_cstr("Cesar");
    String s2 = String_from_cstr("Augustus");
    String s3 = String_from_cstr("");

    rc = String_assign(s3, s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(String_compare(s3, s2) == 0, "s3 != s2");

    rc = String_assign(s3, s1);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(String_compare(s3, s1) == 0, "s3 != s1");

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);

    return TEST_OK;
}

int test_string_copy(void)
{
    String s1 = String_from_cstr("Chopin");
    String s2 = String_copy(s1);
    test(s2 != NULL, "s2 == NULL");
    test(strncmp(s2->data, "Chopin", 7) == 0, "s2->data = '%s'", s1->data);

    String_delete(s1);
    String_delete(s2);

    return TEST_OK;
}

int test_string_append(void)
{
    String s1 = String_from_cstr("spicey");
    String s2 = String_from_cstr(" bolognese");

    rc = String_append(s1, s2);
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s1->data, "spicey bolognese", 17) == 0, "s1->data = '%s'", s1->data);
    test(s1->size == 16, "s1->size = %lu (%lu)", s1->size, 16lu);
    test(s1->capacity == 32, "s1->capacity = %lu (%lu)", s1->capacity, 32lu);

    rc = String_append_cstr(s1, " from Bologna");
    test(rc == 0, "rc = %d (%d)", rc, 0);
    test(strncmp(s1->data, "spicey bolognese from Bologna", 30) == 0,
            "s1->data = '%s'", s1->data);
    test(s1->size == 29, "s1->size = %lu (%lu)", s1->size, 29lu);

    String_delete(s1);
    String_delete(s2);
}

int test_string_concat(void)
{
    String s1 = String_from_cstr("abstract");
    String s2 = String_from_cstr(" algebra");
    String s3 = String_concat(s1, s2);
    test(s3 != NULL, "s3 == NULL");
    test(strncmp(s3->data, "abstract algebra", 17) == 0, "s3->data = '%s'", s3->data);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);

    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_string_from_cstr);
    run_test(test_string_compare);
    run_test(test_string_assign);
    run_test(test_string_copy);
    run_test(test_string_append);
    run_test(test_string_concat);
    test_suite_end();
}
