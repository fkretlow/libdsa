# Queue

[`queue.h`](./../src/queue.h)

Simple FIFO queue, implemented in terms of a doubly-linked list.

- Fast adding at the start and removing at the end in O(1).

```C
// Initialize with the element size and an optional callback for element destruction.
Queue q;
Queue_init(&q, sizeof(int), NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Queue_push(&q, &i);
}

// Pass an address if you want to store popped values.
int value;
while (Queue_size(&q) > 0) {
    Queue_pop(&q, &value);
    // Do stuff with value.
}

// There's no memory block allocated anywhere, but all elements are on the heap.
// Clear non-empty queues when you're done to return the memory.
Queue_clear(&q);
```
