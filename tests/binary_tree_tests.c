#include <stdio.h>
#include "log.h"
#include "check.h"

#include "binary_tree.h"
#include "type_interface.h"

static bt *T;
static int rc;

#define TEST_OK    0;
#define TEST_ERROR 1;

int test_bt_initialize(void)
{
    T = bt_new(NONE, &int_type, NULL);
    check(T != NULL, "T == NULL");
    return TEST_OK;
error:
    return -1;
}

int main(void)
{
    test_bt_initialize();
    return 0;
}
