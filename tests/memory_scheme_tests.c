#include "memory_scheme.h"
#include "type_interface.h"
#include "test.h"


int test_generate_memory_scheme(void)
{
    struct mscheme s;
    int rc = generate_memory_scheme(&s, &int_type, NULL);

    test(rc == 1, "failed to generate memory scheme");
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_generate_memory_scheme);
    test_suite_end();
}
