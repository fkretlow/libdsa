# Deque

[`deque.h`](./../src/deque.h), [`deque.c`](./../src/deque.c)

- Fast adding and removing of elements on both ends in O(1).
- Fast random access in O(1).
- Inserting elements at random positions is not supported.

The double-ended queue is a an abstract data structure that allows for fast insertion and retrieval of elements on both ends. It can be implemented in terms of lists and arrays. If using lists, insertion and deletion at random positions are fast as well, but only sequential access is possible.

This implementation uses arrays, so we have random access in O(1) and good memory locality, but insertion other than at the ends is not supported. All values are copied into the internal storage when they're added, and into memory at the callsite when requested with the getters. It's also possible to request pointers to objects in the deque. There's basic support for complex types that need to do their own memory management on creation and destruction.

```C
// Initialize with the element size and optional copy and destruction callbacks.
Deque d;
Deque_init(&d, sizeof(int), NULL, NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Deque_push_back(&d, &i); // or push_front
}

// Pass an address if you want to store popped values.
int value = 0;
while (Deque_count(&d)) {
    Deque_pop_front(&d, &value);
    // Have fun with value.
}

// Don't forget to return the storage when you're done.
Deque_destroy(&d);
```
