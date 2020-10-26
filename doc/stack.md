# Stack

[`stack.h`](./../src/stack.h), [`stack.c`](./../src/stack.c)

Simple stack (LIFO queue), implemented in terms of a singly-linked list.

- Fast adding and removing of elements at one side in O(1).

```C
// Pass the element size and, optionally, a copy constructor and a destructor for initialization.
Stack S = Stack_new(sizeof(int), NULL, NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Stack_push(S, &i);
}

// Pass an address if you want to store popped values.
int out;
while (!Stack_empty(S)) {
    Stack_pop(S, &out);
    do_stuff_with(out);
}

// Return the memory when you're done.
Stack_delete(S);
```
