#ifndef _bst_h
#define _bst_h

#include <stdint.h>

#include "type_interface.h"

struct bst_node_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct rb_node_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    unsigned char color     : 1;
};

struct avl_node_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    char balance            : 3;
};

struct bst_node;
typedef struct bst_node {
    struct bst_node *parent;
    struct bst_node *left;
    struct bst_node *right;
    union {
        struct bst_node_flags   plain;
        struct rb_node_flags    rb;
        struct avl_node_flags   avl;
    } flags;
} bst_node;

enum bst_flavors { NONE = 0, RB = 1, AVL = 2 };

typedef struct bst {
    bst_node *  root;
    uint32_t    count;
    uint8_t     flavor;
    t_intf *    key_type;
    t_intf *    value_type;
} bst;

/* Interface */

int     bst_initialize           (bst *T, uint8_t flavor, t_intf *kt, t_intf *vt);
bst *   bst_new                  (        uint8_t flavor, t_intf *kt, t_intf *vt);
void    bst_destroy              (bst *T);
void    bst_delete               (bst *T);

void    bst_clear                (bst *T);
bst *   bst_copy                 (          const bst *src);
int     bst_copy_to              (bst *dest, const bst *src);

int     bst_insert               (      bst *T, const void *k);
int     bst_remove               (      bst *T, const void *k);
int     bst_set                  (      bst *T, const void *k, const void *v);
void *  bst_get                  (      bst *T, const void *k);
int     bst_has                  (const bst *T, const void *k);

int     bst_traverse_keys        (bst *T, int (*f)(void *k,     void *p), void *p);
int     bst_traverse_keys_r      (bst *T, int (*f)(void *k,     void *p), void *p);
int     bst_traverse_values      (bst *T, int (*f)(void *v,     void *p), void *p);
int     bst_traverse_values_r    (bst *T, int (*f)(void *v,     void *p), void *p);
int     bst_traverse_nodes       (bst *T, int (*f)(bst_node *n, void *p), void *p);
int     bst_traverse_nodes_r     (bst *T, int (*f)(bst_node *n, void *p), void *p);

int     bst_invariant            (const bst *T);

#define bst_count(T) (T)->count

/* Node subroutines */

bst_node *  bst_node_new                 (const bst *T, const void *k, const void *v);
void        bst_node_delete              (const bst *T, bst_node *n);
void        bst_node_delete_rec          (const bst *T, bst_node *n);

bst_node *  bst_node_copy_rec            (const bst *T, const bst_node *n);

bst_node *  bst_node_insert              (bst *T, bst_node *n, const void *k, const void *v);
bst_node *  bst_node_remove              (bst *T, bst_node *n, const void *k);
bst_node *  bst_node_remove_min          (bst *T, bst_node *n);

#define bst_node_data_size(T) \
    (t_size((T)->key_type) + ((T)->value_type ? t_size((T)->value_type) : 0))
#define bst_node_size(T) (sizeof(bst_node) + bst_node_data_size(T))

#define bst_node_has_key(n)   ((n)->flags.plain.has_key)
#define bst_node_has_value(n) ((n)->flags.plain.has_value)

#define bst_node_key(T, n) (void*)(((char *)(n)) + sizeof(bst_node))
#define bst_node_value(T, n) \
    ((T)->value_type ? (void*)((char *)(n)) + sizeof(bst_node) + t_size((T)->key_type) : NULL)

void    bst_node_set_key            (const bst *T, bst_node *n, const void *k);
void    bst_node_destroy_key        (const bst *T, bst_node *n);
void    bst_node_set_value          (const bst *T, bst_node *n, const void *v);
void    bst_node_destroy_value      (const bst *T, bst_node *n);

void    bst_node_rotate_left        (bst *T, bst_node *n, bst_node **n_out);
void    bst_node_rotate_right       (bst *T, bst_node *n, bst_node **n_out);
void    bst_node_replace_child      (bst *T, bst_node *p, bst_node *c, bst_node *s);

int     bst_node_traverse           (        bst_node *n, int (*f)(bst_node *n,  void *p), void *p);
int     bst_node_traverse_r         (        bst_node *n, int (*f)(bst_node *n,  void *p), void *p);
int     bst_node_traverse_keys      (bst *T, bst_node *n, int (*f)(void *k, void *p), void *p);
int     bst_node_traverse_keys_r    (bst *T, bst_node *n, int (*f)(void *k, void *p), void *p);
int     bst_node_traverse_values    (bst *T, bst_node *n, int (*f)(void *v, void *p), void *p);
int     bst_node_traverse_values_r  (bst *T, bst_node *n, int (*f)(void *v, void *p), void *p);

/* Red-black tree subroutines */

enum rb_colors { BLACK = 0, RED = 1 };

int         rb_invariant            (const bst *T);
bst_node *  rb_node_insert          (bst *T, bst_node *n, const void *k, const void *v);
bst_node *  rb_node_remove          (bst *T, bst_node *n, const void *k);

#endif /* _bst_h */
