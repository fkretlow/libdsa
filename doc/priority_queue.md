# Priority Queue

[`priority_queue.h`](./../src/priority_queue.h), [`priority_queue.c`](./../src/priority_queue.c)  
[`vector.h`](./../src/vector.h), [`vector.c`](./../src/vector.c)  
[`heap.h`](./../src/heap.h), [`heap.c`](./../src/heap.c)

Sequential data structure that always yields the next highest value. Implemented in terms of a heap on a vector. Adding and removal of elements in O(log n).

```C
#include "priority_queue.h"
#include "type_interface.h"

pqueue Q = pqueue_new(&int_type);       /* Q holds objects of type int */

int v;
while (values_exist()) {
    v = some_value();
    int rc = pqueue_enqueue(Q, &v);     /* pass pointers to values you want to enqueue */
}                                       /* rc < 0 on error */

while (!pqueue_empty(Q)) {
    int rc = pqueue_dequeue(Q, &v);     /* the next value is moved to v */
}

pqueue_delete(Q);
```
