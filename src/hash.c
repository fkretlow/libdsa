#include "hash.h"

uint32_t jenkins_hash(const void *obj, const size_t size)
{
    uint32_t hash = 0;

    for (uint32_t i = 0; i < size; ++i) {
        hash += ((unsigned char *)obj)[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
