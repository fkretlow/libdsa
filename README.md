# libdsa

An assortment of commonly used data structures and a few standard algorithms, implemented in C.

1. [Data structures](#data-structures)
2. [Algorithms](#algorithms)
3. [Utilities](#utilities)

## Data Structures

The library provides a selection of commonly used data structures:

Container | Properties | Implemented in terms of
--------- | ---------- | --------
[String](./doc/string.md) | (somewhat) safe string type | dynamic array
[Vector](./doc/vector.md) | random access and fast appending | dynamic array
[List](./doc/list.md) | fast access at the ends | doubly-linked list
[Queue](./doc/queue.md) | FIFO queue | doubly-linked list
[Stack](./doc/stack.md) | LIFO queue | singly-linked list
[Priority Queue](./doc/priority_queue.md) | always yields the next-greatest element | heap on a dynamic array
[Hashmap](./doc/hashmap.md) | stores key-value pairs | hash table with chaining
[Map](./doc/map.md) | stores key-value pairs | balanced binary search tree
[Set](./doc/set.md) | collection of unique elements | balanced binary search tree

The most sophisticated yet somewhat hidden part of the library is the generic [binary search
tree](./src/bst.h) that can be used with two classic balancing strategies: Red-Black and AVL. It
serves as a basis for containers like map and set that require fast lookup of keys with a defined
ordering.

#### Handling Types Generically
The notion of a [*type interface*](./src/type_interface.h) allows to handle arbitrary data types
somewhat generically. (A type interface is just a struct that contains function pointers for
construction, destruction, swap, and move operations on a specific user-defined "type". For
production it would be easy enough to modify this "academic" code to make it work with specific
types in a less hazardous, type-safe manner.)

```C
#include "vector.h"
#include "type_interface.h"

vector *v = vector_new(&int_type); /* v is a vector that stores integers */
```

#### Common Interface
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


## Algorithms
Currently a rather short list: the three most common classic sorting algorithms. They're all
declared in [`sort.h`](./src/sort.h).

1. [Mergesort](./src/mergesort.c)
2. [Quicksort](./src/quicksort.c)
3. [Heapsort](./src/heapsort.c)

## Utilities
- **Error handling:** A couple of macro definitions in [`check.h`](./src/check.h) that allow for easy checking of and reacting to error conditions.
- **Logging:** [`log.h`](./src/log.h)/[`log.c`](./src/log.c) provide fancy colorful, otherwise pretty standard logging utilities.
- **Testing:** A very simple testing framework defined in [`./tests/test.h`](./tests/test.h) that powers the unit tests. Building with just `$ make` rebuilds the library, runs the tests and generates coverage info in `./cov`.
