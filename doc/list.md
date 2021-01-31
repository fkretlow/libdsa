# List

[`list.h`](./../src/list.h), [`list.c`](./../src/list.c)  
[`forward_list.h`](./../src/forward_list.h), [`forward_list.c`](./../src/forward_list.c)

"You ain't a programmer before you've made your own linked list in C." – Well, here's mine and
there's nothing special about it. Fast access at both ends in O(1), but O(n) in the middle. Once a
position is found, insertion and deletion are O(1).

There's also a singly-linked list (called forward list, type `flist`) that only supports iteration in one direction.

```C
#include "list.h"
#include "str.h"
#include "type_interface.h"

list *L = list_new(&int_type);      /* L holds objects of type int */

for (int i = 0; i < 8; ++i) {
    int rc = List_push_back(L, &i); /* pass pointers to objects you want to copy into the list */
}                                   /* rc < 0 on error */

int *ip;
for (list_n *n = L->first; n != NULL; n = n->next) {
    ip = list_n_data(n);            /* basic iteration, the other way around with L->last, n->prev */
}

ip = list_get(L, 0);                /* ip points into the list */

int out;
rc = list_pop_back(L, &out);        /* the last value is moved to out, rc == 0 if L was empty */

list_delete(L);
```
