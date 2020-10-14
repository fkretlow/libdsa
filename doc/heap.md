# Heap

[`heap.h`](./../src/heap.h), [`heap.c`](./../src/heap.c)

The functions defined in these files are used to arrange the elements of an array in an order that satisfies the heap property: Assuming an embedded binary tree in the array, no node has a value that is greater than the value of its parent node. The heap property guarantees a week but sufficient ordering for priority queues: The first element of the array is always the greatest one (or the smallest, depending on the comparison function). Heaps are the key part of the heapsort algorithm.

The functions are meant to be used as low level tools in more abstract data structures. In order to be able to handle array elements of arbitrary sizes, they operate on pointers to chars.

```C
int temp;
int numbers[16];
// populate the array

make_heap((char*)numbers, 16, sizeof(int), compint, &temp);
assert(is_heap((char*)numbers, 16, sizeof(int), compint));
```
