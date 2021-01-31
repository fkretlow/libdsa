# String

[`str.h`](./../src/str.h), [`str.c`](./../src/str.c)

A string type that is somewhat safer than C's character array in that it stores the length of the
string so it doesn't rely on a terminating null character. Implemented as a dynamic array of
characters. Short strings are stored inside the top-level struct itself to avoid unnecessary
memory allocations.

```C
#include "str.h"

str *who = str_from_cstr("Vincent van Gogh");   /* create new string on the heap from C-string literal */
int rc = str_append_cstr(who, " (1853-1890)");  /* append C-string */
                                                /* etc., see str.h */
printf("%s\n", str_data(who));                  /* str_data yields a pointer to the underlying character array */
                                                /* which contains a null byte so it can be used normally */
str_delete(who);
```
