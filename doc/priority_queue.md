# Priority Queue

[`priority_queue.h`](./../src/priority_queue.h), [`priority_queue.c`](./../src/priority_queue.c)

Sequential data structure that always yields the next highest value. Implemented in terms of a heap on a [vector](./vector.md).

- Adding and removal of elements in O(log n).

```C
// Initialize with the element size, an optional callback for element destruction,
// and a callback for element comparison.
PriorityQueue q;
PriorityQueue_init(&q, sizeof(int), NULL, compint);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    PriorityQueue_enqueue(&q, &i);
}

// Pass an address if you want to store popped values.
int value;
while (PriorityQueue_size(&q) > 0) {
    PriorityQueue_dequeue(&q, &value); // get the next highest value
    // Do stuff with value.
}

// Free the allocated storage when you're done.
PriorityQueue_destroy(&q);
```
