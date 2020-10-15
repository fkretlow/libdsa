# Map

[`map.h`](./../src/map.h), [`map.c`](./../src/map.c)

Associative data structure that maps values to keys. Implemented in terms of a hash table with chaining. If the provided hash function generates evenly distributed hashes, this allows for addition and removal of key:value pairs in almost constant time.

```C
// Initialize with object sizes for keys and values,
// a hash and a comparison function for keys,
// and optional destructors for keys and values.
Map M;
Map_init(&M, sizeof(int), sizeof(int), jenkins_hash, compint, NULL, NULL);

// Pass pointers to elements you want to add or get.
Map_set(&M, &key, &value);
int found = Map_has(&M, &key);
Map_get(&M, &key, &value_out);

// Destroy the map to return the memory.
Map_destroy(&M);
```
