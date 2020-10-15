#include "hash.h"

uint32_t jenkins_hash(const void *data, const size_t size)
{
    uint32_t hash = 0;
    uint32_t i = 0;

    for (hash = i = 0; i < size; ++i) {
        hash += ((char *)data)[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
