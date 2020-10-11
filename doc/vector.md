# Vector

[`vector.h`](./../src/vector.h), [`vector.c`](./../src/vector.c)

- Fast adding of elements at the end in O(1).
- Fast random access in O(1).

The term vector is borrowed from the STL. It’s a dynamic array that expands and contracts dynamically if necessary when elements are added or removed.

```C
// Initialize with the element size and an optional callback for element destruction.
Vector v;
Vector_init(&v, sizeof(int), NULL);

// Pass pointers to values you want to add.
for (int i = 0; i < max; ++i) {
    Vector_push_back(&v, &i);
}

// Pass an address if you want to store popped values.
int value;
while (Vector_size(&d) > 0) {
    Vector_pop_back(&v, &value);
    // Have fun with value.
}

// Don't forget to return the data storage when you're done.
Vector_destroy(&v);
```
