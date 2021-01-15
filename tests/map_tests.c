#include <stdio.h>

#include "map.h"
#include "str.h"
#include "test.h"
#include "type_interface.h"

/* Almost the entire functionality of the map is tested by the generic bst tests. */

int test_map_remove(void)
{
    map *M = map_new(&str_type, &int_type);

    str *keys[] = {
        str_from_cstr("oak"),
        str_from_cstr("birch"),
        str_from_cstr("maple")
    };
    for (int i = 0; i < 3; ++i) map_set(M, keys[i], &i);

    int v, rc;

    rc = map_remove(M, keys[0], &v);
    test(rc == 1);
    test(v == 0);
    test(map_has(M, keys[0]) == 0);

    rc = map_remove(M, keys[1], NULL);
    test(rc == 1);
    test(map_has(M, keys[1]) == 0);

    rc = map_remove(M, keys[0], NULL);
    test(rc == 0);

    for (int i = 0; i < 3; ++i) str_delete(keys[i]);
    map_delete(M);
    return 0;
}

int main(void)
{
    test_suite_start();
    run_test(test_map_remove);
    test_suite_end();
}
