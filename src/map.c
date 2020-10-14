#include "debug.h"
#include "map.h"

static inline int __MapNode_new(__MapNode** node_out)
{
    __MapNode* new = calloc(1, sizeof(*new));
    check_alloc(new);
    *node_out = new;
    return 0;
error:
    return -1;
}

static inline void __MapNode_delete(const Map* m, __MapNode* n)
{
    if (n) {
        if (n->key && m && m->destroy_key) {
            m->destroy_key(n->key);
        }
        if (n->value && m && m->destroy_value) {
            m->destroy_value(n->value);
        }
    }
    free(n->key);
    free(n->value);
    free(n);
}

int Map_init(Map* m, const size_t key_size, const size_t value_size,
             __hash_f hash, __compare_f compare,
             __destroy_f destroy_key, __destroy_f destroy_value)
{
    check_ptr(m);
    check_ptr(hash);
    check_ptr(compare);

    m->key_size = key_size;
    m->value_size = value_size;
    m->hash = hash;
    m->compare = compare;
    m->destroy_key = destroy_key;
    m->destroy_value = destroy_value;

    m->n_buckets = MAP_N_BUCKETS;
    m->buckets = calloc(m->n_buckets, sizeof(*m->buckets));
    check_alloc(m->buckets);

    return 0;
error:
    if (m->buckets) free(m->buckets);
}

void Map_clear(Map* m)
{
    __MapNode* cur;
    __MapNode* next;
    for (size_t i = 0; i < m->n_buckets; ++i) {
        cur = m->buckets[i];
        while (cur) {
            next = cur->next;
            __MapNode_delete(m, cur);
            cur = next;
        }
        m->buckets[i] = NULL;
    }
}
