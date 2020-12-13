# List

[`list.h`](./../src/list.h), [`list.c`](./../src/list.c)

"You ain't a programmer before you've made your own linked list in C." – Well, here's mine and there's nothing special about it. Fast adding of elements and acces at both ends in O(1), but O(n) in the middle.

```C
#include "list.h"
#include "str.h"
#include "type_interface.h"

list *L = list_new(&str_type);      /* L holds objects of type str (provided with the library) */

str *s = str_from_cstr("Werner Heisenberg");
int rc = list_push_back(L, s);      /* pass pointers to objects you want to copy into the list */
                                    /* rc < 0 on error */

str *sp = list_get(L, 0);           /* sp != s, sp points to the duplicate of s in the list */

str out;
rc = list_pop_back(L, &out);        /* now out holds the duplicate, rc == 0 if L was empty */

str_delete(s);
str_destroy(out);
list_delete(L);
```
