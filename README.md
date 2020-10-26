# libdsa – Common Data Structures and Algorithms in C

This is a collection of common data structures and algorithms, implemented in
the C programming language for learning purposes. The implementations are kept
simple so everybody with basic knowledge of the C language can read and
understand the code.

The implementations are influenced by the examples in Zed A. Shaw's *Learn C
the Hard Way*, but each of them was rewritten from scratch. The same goes for
the testing and debugging macros in `debug.h` and `test.h`.

Suggestions are welcome and appreciated. Keep in mind that this is a study
project.


## Data structures

The library provides a number of data containers for common applications. They are designed following these principles:

#### Abstraction
Provide a simple to use and consistent interface and hide as much of the necessary low level work behind the scenes. In other words, favor code aesthetics and ease of use over utmost efficiency. For example, the container structs are always allocated on the heap and accessed via pointers, but the fact that a `List` is actually a `_list*` is considered a private implementation detail. The resulting user code looks clean but probably not much like C anymore.

#### Consistency
The interface consists of as small a number of methods as possible for each container. They are named consistently like so:
```C
List List_new(size_t element_size, copy_f copy_element, delete_f delete_element);
void List_delete(List L);
void List_clear(List L);
size_t List_size(const List L);
bool List_empty(const List L);
int List_push_back(List L, void *element);
int List_pop_back(List L, void *element_out);
```
Returned `int`s are success or failure codes, typically `0` on success and a negative value on error. Returned container types are pointers behind the scenes, they're `NULL` on error.

#### Arbitrary element types
Every element is copied into the container when it is added and deleted when it is removed. If provided, the implementation uses user-defined copy constructors, destructors and comparison functions with the following signatures that can be passed to the containers during initialization. Otherwise it falls back to `memcpy` and `free`. The comparison function is always needed for the associative containers.
```C
typedef void (*copy_f)(void *dest, const void *src);
typedef void (*delete_f)(void *element);
typedef int (*compare_f)(const void *a, const void *b);
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
