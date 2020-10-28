#ifndef _hash_h
#define _hash_h

#include <stddef.h>

unsigned long jenkins_hash(const void *obj, const size_t size);

#endif // _hash_h
