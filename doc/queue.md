# Queue

[`queue.h`](./../src/queue.h)

Simple FIFO queue, implemented in terms of a doubly-linked list. Fast adding at the start and removing at the end in O(1).

```C
#include "queue.h"

queue *Q = queue_new(&int_type);        /* Q holds objects of type int */

for (int i = 0; i < 8; ++i) {
    rc = queue_enqueue(Q, &i);          /* pass pointers to values you want to add */
}                                       /* rc < 0 on error */

int *next = queue_next(Q);              /* next points into the queue */

int out;
while (!queue_empty(Q)) {
    rc = queue_dequeue(Q, &out);        /* the next value is moved to out */
}                                       /* again, rc < 0 on error

queue_delete(Q);
```
