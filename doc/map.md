# Map

[`map.h`](./../src/map.h), [`map.c`](./../src/map.c)

Associative data structure that maps values to keys. Implemented in terms of a red-black tree, so
search, insertion and removal are all O(log n).

```C
#include "str.h"
#include "type_interface.h"
#include "map.h"

map *M = map_new(&str_type, &int_type);     /* M maps integers to strings */

str *k = str_from_cstr("Galileo Galilei");
int v = 1564;
int rc = map_set(M, k, &v);                 /* pass pointers to keys/values */
                                            /* rc < 0 on error */

rc = map_has(M, k);                         /* membership test */
int *vp = map_get(M, k);                    /* vp points into the map */

rc = map_remove(M, k, &v);                  /* now v holds the value previously mapped to k */
                                            /* rc < 0 on error, rc == 0 if k wasn't found */

str_delete(k);
map_delete(M);
```
