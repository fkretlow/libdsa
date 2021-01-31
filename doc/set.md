# Set

[`set.h`](./../src/set.h), [`set.c`](./../src/set.c)  
[`bst.h`](./../src/bst.h), [`bst.c`](./../src/bst.c), [`rb.c`](./../src/rb.c)

Implementation of the set abstraction: A container that holds unique elements. Implemented in
terms of a red-black tree, which gives lookup, insertion and removal in O(log n).

```C
#include "set.h"

set *S = set_new(&int_type);        /* S holds unique integers */

while (values_exist()) {
    int v = some_value();
    int rc = set_insert(S, &v);     /* pass pointers to values you want to add */
                                    /* rc == 1 if a value was added, 0 if it was present, -1 on error */
}

int v = some_value();
int rc = set_remove(S, &v);         /* rc == 1 if v was found and removed, 0 if not, -1 on error */
```

#### Set operations
Let S1 and S2 be sets. The standard set operations *union* (OR), *intersection* (AND), and
*difference* yield new sets:
```C
set *U = set_union(S1, S2);
set *I = set_intersection(S1, S2);
set *D = set_difference(S1, S2);
```

#### In-order Traversal
Let *f* be some function of type `int f(void *e, void *p)` that processes each element and
returns a non-zero integer if the traversal should abort. Let *p* be a pointer to something that
should be passed as additional argument to *f* (can be `NULL`).

```C
int rc = set_traverse(S, f, p);     /* do something with every element */
rc = set_traverse_r(S, f, p);       /* do something with every element in reverse order */
                                    /* rc = 0 if the traversal succeeded, otherwise the rv of f */
```
