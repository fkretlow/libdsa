# Vector

[`vector.h`](./../src/vector.h), [`vector.c`](./../src/vector.c)

The basic dynamic array type. Expands and contracts automatically if needed when elements are
added or removed. Adding of elements at the end and random access in O(1).

```C
#include "str.h"
#include "type_interface.h"
#include "vector.h"

vector *V = vector_new(&str_type);      /* v holds objects of type str (provided with the library) */

str *s = str_from_cstr("Isaac Newton");
int rc = vector_push_back(V, s);        /* pass pointers to objects you want to copy into the vector */
                                        /* rc < 0 on error */

str *sp = vector_get(V, 0);             /* sp != s, sp points to the duplicate of s in the vector */

str out;
rc = vector_pop_back(V, &out);          /* now out holds the duplicate, rc == 0 if V was empty */

str_delete(s);
str_destroy(out);
vector_delete(V);
```
