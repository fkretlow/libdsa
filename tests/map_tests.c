#include "debug.h"
#include "unittest.h"
#include "map.h"

static Map M;
static int rc;

int test_map_init(void)
{
    return TEST_OK;
}

int test_map_usage(void)
{
    return TEST_OK;
}

int test_map_clear(void)
{
    return TEST_OK;
}

int test_map_destroy(void)
{
    return TEST_OK;
}

int main(void)
{
    test_suite_start();
    run_test(test_map_init);
    run_test(test_map_usage);
    run_test(test_map_clear);
    run_test(test_map_destroy);
    test_suite_end();
}
