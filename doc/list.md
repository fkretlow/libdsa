# List

[`list.h`](./../src/list.h), [`list.c`](./../src/list.c)

- Fast adding of elements at both ends in O(1).
- Fast access at both ends in O(1), but O(n) in the middle.
- Small chunks of memory all over the place.

"You ain't a programmer before you've made your own linked list in C." – Well, here's mine and there's nothing special about it.

```C
// Initialize with the element size and an optional callback for element destruction.
List l;
List_init(&l, sizeof(int), NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    List_push_back(&v, &i); // or push_front
}

// Pass an address if you want to store popped values.
int value;
while (List_size(&l) > 0) {
    List_pop_back(&d, &value); // or pop_front
    // Do stuff with value.
}

// There's no memory block allocated anywhere, but all elements are on the heap.
// Clear the list when you're done to return the memory.
List_clear(&l);
```
