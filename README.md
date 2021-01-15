# libdsa

## Data structures

The library provides a selection of commonly used data structures. The notion of a *type
interface* allows to handle arbitrary data types somewhat generically. (A type interface is just a
struct that contains function pointers for construction, destruction, swap, and move operations on
a specific user-defined "type". For production it would be easy enough to modify this "academic"
code to make it work with specific types in a less hazardous, type-safe manner.)

```C
#include "vector.h"
#include "type_interface.h"

vector *v = vector_new(&int_type); /* v is a vector that stores integers */
```

All data objects that are inserted into one of the containers are copied into the container.
Whether it's a deep copy depends on the copy constructor given with the type interface.
```C
int i = 1;
vector_push_back(V, &i); /* i is copied into the vector */
```
Normal get operations return pointers to the objects inside the container.
```C
int *ip = vector_get(V, 0); /* ip points into the vector */
```

Operations that remove objects from a container take an optional memory address as argument. If
that is given, the object is moved there instead of being destroyed.
```C
vector_pop_back(V, &i); /* the last element in v is moved to the address of i */
```

### Available containers

Container | Properties | Implemented in terms of
--------- | ---------- | --------
[String](./doc/string.md) | (somewhat) safe string type | dynamic array
[Vector](./doc/vector.md) | random access and fast appending | dynamic array
[List](./doc/list.md) | fast access at the ends | doubly-linked list
[Queue](./doc/queue.md) | FIFO queue | doubly-linked list
[Stack](./doc/stack.md) | LIFO queue | singly-linked list
[Priority Queue](./doc/priority_queue.md) | always yields the next-greatest element | heap on a dynamic array
[Map](./doc/map.md) | stores key-value pairs | hash table with chaining
[Set](./doc/set.md) | collection of unique elements | balanced binary search tree

## Algorithms

1. [Mergesort](./src/mergesort.c)
2. [Quicksort](./src/quicksort.c)
3. [Heapsort](./src/heapsort.c)
