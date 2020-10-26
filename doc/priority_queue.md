# Priority Queue

[`priority_queue.h`](./../src/priority_queue.h), [`priority_queue.c`](./../src/priority_queue.c)

Sequential data structure that always yields the next highest value. Implemented in terms of a heap on a [vector](./vector.md).

- Adding and removal of elements in O(log n).

```C
// Initialize with the element size and an optional callback for element destruction. A comparison function is always needed.
PriorityQueue Q = PriorityQueue_new(sizeof(int), NULL, NULL, compint);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    PriorityQueue_enqueue(Q, &i);
}

// Pass an address if you want to store dequeued elements. You always get the next-greatest one.
int out;
while (!PriorityQueue_empty(Q)) {
    PriorityQueue_dequeue(Q, &out);
    do_stuff_with(out);
}

// Free the allocated storage when you're done.
PriorityQueue_delete(Q);
```
