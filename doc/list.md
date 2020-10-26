# List

[`list.h`](./../src/list.h), [`list.c`](./../src/list.c)

"You ain't a programmer before you've made your own linked list in C." – Well, here's mine and there's nothing special about it.

- Fast adding of elements at both ends in O(1).
- Fast access at both ends in O(1), but O(n) in the middle.
- Small chunks of memory all over the place.

```C
// Pass the element size and, optionally, a copy constructor and a destructor for initialization.
List L = List_new(sizeof(int), NULL, NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    List_push_back(L, &i);
}

// Pass an address if you want to store popped values.
int out;
while (!List_empty(L)) {
    List_pop_back(L, &out);
    do_stuff_with(out);
}

// Return the memory when you're done.
List_delete(L);
```
