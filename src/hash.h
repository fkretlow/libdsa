#ifndef _hash_h
#define _hash_h

#include <stdint.h>
#include <stddef.h>

typedef uint32_t (*__hash_f)(const void* data, const size_t size);

uint32_t jenkins_hash(const void *data, const size_t size);

#endif // _hash_h
