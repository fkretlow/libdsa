#include <string.h>

#include "memory_scheme.h"
#include "type_interface.h"
#include "test.h"

static t_intf type_4  = { .size =  4 };
static t_intf type_8  = { .size =  8 };
static t_intf type_12 = { .size = 12 };
static t_intf type_32 = { .size = 32 };

int test_generate_memory_scheme(void)
{
    struct mscheme s;
    int rc = generate_memory_scheme(&s, &type_4, NULL);

    test(rc == 0);
    test(s.key_type == &type_4);
    test(s.value_type == NULL);
    test(s.key_external == 0);

    rc = generate_memory_scheme(&s, &type_12, NULL);
    test(rc == 0);
    test(s.key_external == 0);

    rc = generate_memory_scheme(&s, &type_32, NULL);
    test(rc == 0);
    test(s.key_external == 1);

    rc = generate_memory_scheme(&s, &type_4, &type_4);
    test(rc == 0);
    test(s.key_external == 0);
    test(s.value_external == 0);

    rc = generate_memory_scheme(&s, &type_4, &type_12);
    test(rc == 0);
    test(s.key_external == 0);
    test(s.value_external == 0);

    rc = generate_memory_scheme(&s, &type_8, &type_32);
    test(rc == 0);
    test(s.key_external == 0);
    test(s.value_external == 1);

    rc = generate_memory_scheme(&s, &type_32, &type_4);
    test(rc == 0);
    test(s.key_external == 1);
    test(s.value_external == 0);

    rc = generate_memory_scheme(&s, &type_32, &type_12);
    test(rc == 0);
    test(s.key_external == 1);
    test(s.value_external == 1);

    return 0;
}

int test_key_handlers(void)
{
    int rc;
    struct mscheme s;
    char data[2 * sizeof(char*)];

    generate_memory_scheme(&s, &int_type, NULL);

    int k = 3;
    rc = set_key(data, &s, &k);
    test(rc == 0);
    test(*(int*)data == k);

    int *kp = get_key(data, &s);
    test(kp != NULL);
    test(kp == (int*)data);

    destroy_key(data, &s);
    test(*(int*)data == 0);

    generate_memory_scheme(&s, &type_32, NULL);
    int64_t obj[4] = { 1, 2, 3, 4 };

    rc = set_key(data, &s, obj);
    test(rc == 0);

    int64_t *obj_p = get_key(data, &s);
    for (int i = 0; i < 4; ++i) test(*(obj_p + i) == obj[i]);

    destroy_key(data, &s);
    test(get_key(data, &s) == NULL);

    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_generate_memory_scheme);
    run_test(test_key_handlers);
    test_suite_end();
}
