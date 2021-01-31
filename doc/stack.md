# Stack

[`stack.h`](./../src/stack.h), [`stack.c`](./../src/stack.c)

Simple stack (LIFO queue), implemented in terms of a singly-linked list. Fast adding and removing
of elements at one side in O(1).

```C
#include "stack.h"
#include "type_interface.h"

stack *S = stack_new(&int_type);        /* S holds objects of type int */

for (int i = 0; i < 8; ++i) {
    stack_push(S, &i);                  /* pass addresses of values to add */
}

int *top = stack_top(S);                /* top points into the stack, *top == 7 */

int out;
while (!stack_empty(S)) {
    stack_pop(S, &out);                 /* each value is moved to out */
}

stack_delete(S);
```
