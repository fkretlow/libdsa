/*************************************************************************************************
 *
 * bst.h
 *
 * Interface for a binary search tree that supports different key/value types by way of type
 * interface structs, and different balancing strategies. The BST can store key-value pairs or
 * just keys, and it provides the basis for other abstractions in the library (set, map) that
 * require fast search of keys with a defined ordering.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _bst_h
#define _bst_h

#include <stdint.h>
#include "type_interface.h"

enum bst_flavors { NONE = 0, RB = 1, AVL = 2 };

struct bst_n_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct rb_n_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    unsigned char color     : 1;
};

struct avl_n_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    char balance            : 3;
};

struct bst_n;
typedef struct bst_n {
    struct bst_n *left;
    struct bst_n *right;
    union {
        struct bst_n_flags   plain;
        struct rb_n_flags    rb;
        struct avl_n_flags   avl;
    } flags;
} bst_n;

typedef struct bst {
    bst_n *      root;
    size_t      count;
    uint8_t     flavor;
    t_intf *    key_type;
    t_intf *    value_type;
} bst;

struct bst_stats {
    int height;
    int shortest_path;
    int total_nodes;
    int black_height;
    int black_nodes;
    int red_nodes;
};

/* public interface */

int     bst_initialize          (bst *T, uint8_t flavor, t_intf *kt, t_intf *vt);
bst *   bst_new                 (        uint8_t flavor, t_intf *kt, t_intf *vt);
void    bst_destroy             (bst *T);
void    bst_delete              (bst *T);

void    bst_clear               (bst *T);
bst *   bst_copy                (           const bst *src);
int     bst_copy_to             (bst *dest, const bst *src);

int     bst_insert              (      bst *T, const void *k);
int     bst_remove              (      bst *T, const void *k);
int     bst_set                 (      bst *T, const void *k, const void *v);
void *  bst_get                 (      bst *T, const void *k);
int     bst_has                 (const bst *T, const void *k);

int     bst_traverse_keys       (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_keys_r     (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_values     (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_values_r   (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_nodes      (bst *T, int (*f)(bst_n *n, void *p), void *p);
int     bst_traverse_nodes_r    (bst *T, int (*f)(bst_n *n, void *p), void *p);

int     bst_invariant           (const bst *T, struct bst_stats *s_out);

#define bst_count(T) (T)->count

/*************************************************************************************************
 *
 * Everything below this point is considered a private implementation detail and should not be
 * used by other code. The declarations are included here for testing purposes only.
 *
 ************************************************************************************************/

/* subroutines on normal BST nodes */

bst_n *  bst_n_new                (const bst *T, const void *k, const void *v);
void    bst_n_delete             (const bst *T, bst_n *n);
void    bst_n_delete_rec         (const bst *T, bst_n *n);

bst_n *  bst_n_copy_rec           (const bst *T, const bst_n *n);

bst_n *  bst_n_find               (const bst *T, bst_n *n, const void *k);

int     bst_n_insert             (bst *T, bst_n **np, const void *k, const void *v);
int     bst_n_remove             (bst *T, bst_n **np, const void *k);
int     bst_n_remove_min         (bst *T, bst_n **np);

void    bst_n_set_key            (const bst *T, bst_n *n, const void *k);
void    bst_n_destroy_key        (const bst *T, bst_n *n);
void    bst_n_set_value          (const bst *T, bst_n *n, const void *v);
void    bst_n_destroy_value      (const bst *T, bst_n *n);
void    bst_n_move_data          (const bst *T, bst_n *dest, bst_n *src);

int     bst_n_traverse           (        bst_n *n, int (*f)(bst_n *n, void *p), void *p);
int     bst_n_traverse_r         (        bst_n *n, int (*f)(bst_n *n, void *p), void *p);
int     bst_n_traverse_keys      (bst *T, bst_n *n, int (*f)(void *k, void *p), void *p);
int     bst_n_traverse_keys_r    (bst *T, bst_n *n, int (*f)(void *k, void *p), void *p);
int     bst_n_traverse_values    (bst *T, bst_n *n, int (*f)(void *v, void *p), void *p);
int     bst_n_traverse_values_r  (bst *T, bst_n *n, int (*f)(void *v, void *p), void *p);

size_t  bst_n_height             (const bst_n *n);

#define bst_n_data_size(T) \
    (t_size((T)->key_type) + ((T)->value_type ? t_size((T)->value_type) : 0))
#define bst_n_size(T) (sizeof(bst_n) + bst_n_data_size(T))

#define bst_n_has_key(n)   ((n)->flags.plain.has_key)
#define bst_n_has_value(n) ((n)->flags.plain.has_value)

#define bst_n_key(T, n) (void*)(((char *)(n)) + sizeof(bst_n))
#define bst_n_value(T, n) \
    ((T)->value_type ? (void*)((char *)(n)) + sizeof(bst_n) + t_size((T)->key_type) : NULL)

/* RB node subroutines */

enum rb_colors { RED = 0, BLACK = 1 };

int rb_n_invariant   (const bst *T, const bst_n *n, int depth, int black_depth, struct bst_stats *s);
int rb_n_insert      (bst *T, bst_n **np, const void *k, const void *v);
int rb_n_remove      (bst *T, bst_n **np, const void *k);

/* AVL node subroutines */

int avl_n_invariant  (const bst *T, const bst_n *n, int depth, int *height_out, struct bst_stats *s);
int avl_n_insert     (bst *T, bst_n **np, const void *k, const void *v, short *dhp);
int avl_n_remove     (bst *T, bst_n **np, const void *k, short *dhp);

#endif /* _bst_h */
