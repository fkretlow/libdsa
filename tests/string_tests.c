#include <stdbool.h>

#include "log.h"
#include "test.h"
#include "str.h"
#include "type_interface.h"

static int rc;

int test_string_from_cstr(void)
{
    str *s = str_from_cstr("Hello World");
    test(s != NULL);
    test(strncmp(str_data(s), "Hello World", 11) == 0);
    test(s->length == 11);
    test(str_data(s)[11] == '\0');
    if (STR_INTERNAL_CAPACITY > 12) {
        test(s->data_external == false);
    }
    str_delete(s);
    return 0;
}

int test_string_compare(void)
{
    str *s1 = str_from_cstr("foo");
    str *s2 = str_from_cstr("foo");
    str *s3 = str_from_cstr("bar");

    rc = str_compare(s1, s2);
    test(rc == 0);

    rc = str_compare(s1, s3);
    test(rc > 0);

    str_delete(s1);
    str_delete(s2);
    str_delete(s3);
    return 0;
}

int test_string_assign(void)
{
    str *s1 = str_from_cstr("Cesar");
    str *s2 = str_from_cstr("Augustus");
    str *s3 = str_from_cstr("");

    rc = str_assign(s3, s2);
    test(rc == 0);
    test(str_compare(s3, s2) == 0);

    rc = str_assign(s3, s1);
    test(rc == 0);
    test(str_compare(s3, s1) == 0);

    str_delete(s1);
    str_delete(s2);
    str_delete(s3);

    return 0;
}

int test_string_copy(void)
{
    str *s1 = str_from_cstr("Chopin");
    str *s2 = str_copy(s1);
    test(s2 != NULL);
    test(strncmp(str_data(s2), "Chopin", 7) == 0);

    str_delete(s1);
    str_delete(s2);

    return 0;
}

int test_string_append(void)
{
    str *s1 = str_from_cstr("spicey");
    str *s2 = str_from_cstr(" bolognese");

    rc = str_append(s1, s2);
    test(rc == 0);
    test(strncmp(str_data(s1), "spicey bolognese", 17) == 0);
    test(s1->length == 16);
    test(str_capacity(s1) == 32);

    rc = str_append_cstr(s1, " from Bologna");
    test(rc == 0);
    test(strncmp(str_data(s1), "spicey bolognese from Bologna", 30) == 0);
    test(s1->length == 29);

    str_delete(s1);
    str_delete(s2);
    return 0;
}

int test_string_concat(void)
{
    str *s1 = str_from_cstr("abstract");
    str *s2 = str_from_cstr(" algebra");
    str *s3 = str_concat(s1, s2);
    test(s3 != NULL);
    test(strncmp(str_data(s3), "abstract algebra", 17) == 0);
    test(s3->data_external == true);

    str_delete(s1);
    str_delete(s2);
    str_delete(s3);

    return 0;
}

int test_string_push_pop_back(void)
{
    str *s = str_new();
    char out;

    rc = str_push_back(s, 'a');
    test(rc == 0);
    test(str_data(s)[0] == 'a');
    test(s->length == 1);

    rc = str_pop_back(s, &out);
    test(rc == 0);
    test(out == 'a');

    str_delete(s);
    return 0;
}

int test_string_allocation(void)
{
    str *s = str_new();
    test(s->data_external == false);

    for (size_t i = 0; i < STR_INTERNAL_CAPACITY - 1; ++i) {
        str_push_back(s, 'x');
    }
    test(s->data_external == false);

    str_push_back(s, 'y');
    test(s->data_external == true);
    test(str_capacity(s) == 16);

    str_shrink_to_fit(s);
    test(s->data_external == true);
    test(str_capacity(s) == 16);

    str_pop_back(s, NULL);
    test(s->data_external == true);
    test(str_capacity(s) == 16);

    str_shrink_to_fit(s);
    test(s->data_external == false);

    str_clear(s);
    test(s->data_external == false);

    str_assign_cstr(s, "This is a very long string, longer than the threshold.");
    test(s->data_external == true);
    test(str_capacity(s) == 64);

    str_clear(s);
    test(s->data_external == false);

    str_delete(s);
    return 0;
}

int test_string_hash(void)
{
    str *s = str_from_cstr("Maurice Ravel");
    unsigned long hash = str_hash(s);
    test(hash != 0);
    str_delete(s);
    return 0;
}

int test_string_type_interface(void)
{
    str *s1 = str_from_cstr("Debussy");
    str *s2 = str_from_cstr("Debussy");

    /* t_print(&str_type, stdout, &s1); */

    rc = t_compare(&str_type, s1, s2);
    test(rc == 0);

    unsigned long h1, h2;
    h1 = t_hash(&str_type, s1);
    h2 = t_hash(&str_type, s2);
    test(h1 == h2);

    str dest;
    t_copy(&str_type, &dest, s1);
    rc = str_compare(&dest, s1);
    test(rc == 0);
    test(dest.length == s1->length);

    t_destroy(&str_type, &dest);

    str_delete(s1);
    str_delete(s2);
    return 0;
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
