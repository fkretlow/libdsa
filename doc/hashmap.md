# Map

[`hashmap.h`](./../src/hashmap.h), [`hashmap.c`](./../src/hashmap.c)

Implementation of the unordered associative array abstraction in terms of a hash table where
key-value pairs are distributed over lists ("buckets") by hashing the key. Provided the given hash
function generates evenly-distributed hashes, get/set operations run in amortized O(1).

```C
#include "hashmap.h"
#include "str.h"
#include "type_interface.h"

hashmap *M = hashmap_new(&str_type, &int_type);     /* M maps integers to strings */

str *k = str_from_cstr("Galileo Galilei");
int v = 1564;
int rc = map_set(M, k, &v);                         /* pass pointers to keys/values */
                                                    /* rc < 0 on error */

rc = map_has(M, k);                                 /* membership test */
int *vp = map_get(M, k);                            /* vp points into the map */

rc = map_remove(M, k);                              /* remove the pair k:v */
                                                    /* rc < 0 on error, rc == 0 if k wasn't found */

str_delete(k);
map_delete(M);
```
