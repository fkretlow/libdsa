/*************************************************************************************************
 *
 * hash.h
 * Jenkins hashing algorithm.
 *
 ************************************************************************************************/

#ifndef _hash_h
#define _hash_h

#include <stdint.h>
#include <stddef.h>

uint32_t jenkins_hash(const void *obj, const size_t size);

#endif /* _hash_h */
