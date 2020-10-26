# Vector

[`vector.h`](./../src/vector.h), [`vector.c`](./../src/vector.c)

The term vector is borrowed from the STL. It’s a dynamic array that expands and contracts automatically if needed when elements are added or removed.

- Fast adding of elements at the end in O(1).
- Fast random access in O(1).

```C
// Pass the element size and, optionally, a copy constructor and a destructor for initialization.
Vector V = Vector_new(sizeof(int), NULL, NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Vector_push_back(V, &i);
}

// Pass an address if you want to store popped values.
int out;
while (!Vector_empty(V)) {
    Vector_pop_back(V, &out);
    do_stuff_with(out);
}

// Return the memory when you're done.
Vector_delete(V);
```
