#include "hash.h"

unsigned long jenkins_hash(const void *obj, const size_t size)
{
    unsigned long hash = 0;

    for (unsigned long i = 0; i < size; ++i) {
        hash += ((char *)obj)[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
