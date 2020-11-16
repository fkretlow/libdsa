#include <stdbool.h>

#include "log.h"
#include "test.h"
#include "str.h"
#include "type_interface.h"

static int rc;

int test_string_from_cstr(void)
{
    String *s = String_from_cstr("Hello World");
    test(s != NULL);
    test(strncmp(String_data(s), "Hello World", 11) == 0);
    test(s->size == 11);
    test(String_data(s)[11] == '\0');
    if (STRING_ALLOC_THRESHOLD > 12) {
        test(s->storage_allocated == false);
    }
    String_delete(s);
    return TEST_OK;
}

int test_string_compare(void)
{
    String *s1 = String_from_cstr("foo");
    String *s2 = String_from_cstr("foo");
    String *s3 = String_from_cstr("bar");

    rc = String_compare(s1, s2);
    test(rc == 0);

    rc = String_compare(s1, s3);
    test(rc > 0);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);
    return TEST_OK;
}

int test_string_assign(void)
{
    String *s1 = String_from_cstr("Cesar");
    String *s2 = String_from_cstr("Augustus");
    String *s3 = String_from_cstr("");

    rc = String_assign(s3, s2);
    test(rc == 0);
    test(String_compare(s3, s2) == 0);

    rc = String_assign(s3, s1);
    test(rc == 0);
    test(String_compare(s3, s1) == 0);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);

    return TEST_OK;
}

int test_string_copy(void)
{
    String *s1 = String_from_cstr("Chopin");
    String *s2 = String_copy(s1);
    test(s2 != NULL);
    test(strncmp(String_data(s2), "Chopin", 7) == 0);

    String_delete(s1);
    String_delete(s2);

    return TEST_OK;
}

int test_string_append(void)
{
    String *s1 = String_from_cstr("spicey");
    String *s2 = String_from_cstr(" bolognese");

    rc = String_append(s1, s2);
    test(rc == 0);
    test(strncmp(String_data(s1), "spicey bolognese", 17) == 0);
    test(s1->size == 16);
    test(String_capacity(s1) == 32);

    rc = String_append_cstr(s1, " from Bologna");
    test(rc == 0);
    test(strncmp(String_data(s1), "spicey bolognese from Bologna", 30) == 0);
    test(s1->size == 29);

    String_delete(s1);
    String_delete(s2);
    return TEST_OK;
}

int test_string_concat(void)
{
    String *s1 = String_from_cstr("abstract");
    String *s2 = String_from_cstr(" algebra");
    String *s3 = String_concat(s1, s2);
    test(s3 != NULL);
    test(strncmp(String_data(s3), "abstract algebra", 17) == 0);
    test(s3->storage_allocated == true);

    String_delete(s1);
    String_delete(s2);
    String_delete(s3);

    return TEST_OK;
}

int test_string_push_pop_back(void)
{
    String *s = String_new();
    char out;

    rc = String_push_back(s, 'a');
    test(rc == 0);
    test(String_data(s)[0] == 'a');
    test(s->size == 1);

    rc = String_pop_back(s, &out);
    test(rc == 0);
    test(out == 'a');

    String_delete(s);
    return TEST_OK;
}

int test_string_allocation(void)
{
    String *s = String_new();
    test(s->storage_allocated == false);

    for (size_t i = 0; i < STRING_ALLOC_THRESHOLD - 1; ++i) {
        String_push_back(s, 'x');
    }
    test(s->storage_allocated == false);

    String_push_back(s, 'y');
    test(s->storage_allocated == true);
    test(String_capacity(s) == 32);

    String_shrink_to_fit(s);
    test(s->storage_allocated == true);
    test(String_capacity(s) == 32);

    String_pop_back(s, NULL);
    test(s->storage_allocated == true);
    test(String_capacity(s) == 32);

    String_shrink_to_fit(s);
    test(s->storage_allocated == false);

    String_clear(s);
    test(s->storage_allocated == false);

    String_assign_cstr(s, "This is a very long string, longer than the threshold.");
    test(s->storage_allocated == true);
    test(String_capacity(s) == 64);

    String_clear(s);
    test(s->storage_allocated == false);

    String_delete(s);
    return TEST_OK;
}

int test_string_hash(void)
{
    String *s = String_from_cstr("Maurice Ravel");
    unsigned long hash = String_hash(s);
    test(hash != 0);
    String_delete(s);
    return TEST_OK;
}

int test_string_type_interface(void)
{
    String *s1 = String_from_cstr("Debussy");
    String *s2 = String_from_cstr("Debussy");

    /* t_print(&String_type, stdout, &s1); */

    rc = t_compare(&String_type, s1, s2);
    test(rc == 0);

    unsigned long h1, h2;
    h1 = t_hash(&String_type, s1);
    h2 = t_hash(&String_type, s2);
    test(h1 == h2);

    String dest;
    t_copy(&String_type, &dest, s1);
    rc = String_compare(&dest, s1);
    test(rc == 0);
    test(dest.size == s1->size);

    t_destroy(&String_type, &dest);

    String_delete(s1);
    String_delete(s2);
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
    run_test(test_string_push_pop_back);
    run_test(test_string_allocation);
    run_test(test_string_hash);
    run_test(test_string_type_interface);
    test_suite_end();
}
