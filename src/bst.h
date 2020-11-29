#ifndef _bst_h
#define _bst_h

#include <stdint.h>

#include "type_interface.h"

enum bst_flavors { NONE = 0, RB = 1, AVL = 2 };

struct bstn_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct rbn_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    unsigned char color     : 1;
};

struct avln_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    char balance            : 3;
};

struct bstn;
typedef struct bstn {
    struct bstn *parent;
    struct bstn *left;
    struct bstn *right;
    union {
        struct bstn_flags   plain;
        struct rbn_flags    rb;
        struct avln_flags   avl;
    } flags;
} bstn;

typedef struct bst {
    bstn *      root;
    uint32_t    count;
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

/* interface */

int     bst_initialize           (bst *T, uint8_t flavor, t_intf *kt, t_intf *vt);
bst *   bst_new                  (        uint8_t flavor, t_intf *kt, t_intf *vt);
void    bst_destroy              (bst *T);
void    bst_delete               (bst *T);

void    bst_clear                (bst *T);
bst *   bst_copy                 (           const bst *src);
int     bst_copy_to              (bst *dest, const bst *src);

int     bst_insert               (      bst *T, const void *k);
int     bst_remove               (      bst *T, const void *k);
int     bst_set                  (      bst *T, const void *k, const void *v);
void *  bst_get                  (      bst *T, const void *k);
int     bst_has                  (const bst *T, const void *k);

int     bst_traverse_keys        (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_keys_r      (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_values      (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_values_r    (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_nodes       (bst *T, int (*f)(bstn *n, void *p), void *p);
int     bst_traverse_nodes_r     (bst *T, int (*f)(bstn *n, void *p), void *p);

int     bst_invariant            (const bst *T, struct bst_stats *s_out);

#define bst_count(T) (T)->count

/* node subroutines */

bstn *  bstn_new                (const bst *T, const void *k, const void *v);
void    bstn_delete             (const bst *T, bstn *n);
void    bstn_delete_rec         (const bst *T, bstn *n);

bstn *  bstn_copy_rec           (const bst *T, const bstn *n);

int     bstn_insert             (bst *T, bstn **np, const void *k, const void *v);
int     bstn_remove             (bst *T, bstn **np, const void *k);
int     bstn_remove_min         (bst *T, bstn **np);

void    bstn_set_key            (const bst *T, bstn *n, const void *k);
void    bstn_destroy_key        (const bst *T, bstn *n);
void    bstn_set_value          (const bst *T, bstn *n, const void *v);
void    bstn_destroy_value      (const bst *T, bstn *n);

void    bstn_rotate_left        (bst *T, bstn *n, bstn **n_out);
void    bstn_rotate_right       (bst *T, bstn *n, bstn **n_out);
void    bstn_replace_child      (bst *T, bstn *p, bstn *c, bstn *s);

int     bstn_traverse           (        bstn *n, int (*f)(bstn *n,  void *p), void *p);
int     bstn_traverse_r         (        bstn *n, int (*f)(bstn *n,  void *p), void *p);
int     bstn_traverse_keys      (bst *T, bstn *n, int (*f)(void *k, void *p), void *p);
int     bstn_traverse_keys_r    (bst *T, bstn *n, int (*f)(void *k, void *p), void *p);
int     bstn_traverse_values    (bst *T, bstn *n, int (*f)(void *v, void *p), void *p);
int     bstn_traverse_values_r  (bst *T, bstn *n, int (*f)(void *v, void *p), void *p);

#define bstn_data_size(T) \
    (t_size((T)->key_type) + ((T)->value_type ? t_size((T)->value_type) : 0))
#define bstn_size(T) (sizeof(bstn) + bstn_data_size(T))

#define bstn_has_key(n)   ((n)->flags.plain.has_key)
#define bstn_has_value(n) ((n)->flags.plain.has_value)

#define bstn_key(T, n) (void*)(((char *)(n)) + sizeof(bstn))
#define bstn_value(T, n) \
    ((T)->value_type ? (void*)((char *)(n)) + sizeof(bstn) + t_size((T)->key_type) : NULL)

/* RB node subroutines */

enum rb_colors { BLACK = 0, RED = 1 };

int rbn_invariant (const bst *T, const bstn *n, int depth, int black_depth, struct bst_stats *s);
int rbn_insert    (bst *T, bstn **np, const void *k, const void *v);
int rbn_remove    (bst *T, bstn **np, const void *k);

#endif /* _bst_h */
