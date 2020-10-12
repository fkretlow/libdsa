# Stack

[`stack.h`](./../src/stack.h), [`stack.c`](./../src/stack.c)

Simple stack (LIFO queue), implemented in terms of a singly-linked list.

- Fast adding and removing of elements at one side in O(1).

```C
// Initialize with the element size and an optional callback for element destruction.
Stack s;
Stack_init(&s, sizeof(int), NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Stack_push(&s, &i);
}

// Pass an address if you want to store popped values.
int value;
while (Stack_size(&s) > 0) {
    Stack_pop(&s, &value);
    // Do stuff with value.
}

// There's no memory block allocated anywhere, but all elements are on the heap.
// Clear non-empty stacks when you're done to return the memory.
Stack_clear(&s);
```
