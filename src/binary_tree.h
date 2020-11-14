#ifndef _binary_tree_h
#define _binary_tree_h

#include <stdint.h>

#include "node_data.h"
#include "type_interface.h"

struct bstn_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
};

struct rbtn_flags {
    unsigned char has_key   : 1;
    unsigned char has_value : 1;
    unsigned char color     : 1;
};

struct avltn_flags {
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
        struct bstn_flags  plain;
        struct rbtn_flags  red_black;
        struct avltn_flags avl;
    } flags;
    char data[MAPPING_DATA_SIZE];
} bstn;

#define NONE 0
#define RED_BLACK 1
#define AVL 2

typedef struct {
    bstn *root;
    uint32_t count;
    struct mem_scheme memory_scheme;
    uint8_t flavor;
} bst;

/* Interface */

int     bst_initialize          (bst *T, uint8_t flavor, t_intf *kt, t_intf *vt);
bst *   bst_new                 (        uint8_t flavor, t_intf *kt, t_intf *vt);
void    bst_destroy             (bst *T);
void    bst_delete              (bst *T);

int     bst_copy                (bst *dest, bst *src);
void    bst_clear               (bst *T);

int     bst_insert              (bst *T, const void *k);
int     bst_remove              (bst *T, const void *k);
int     bst_set                 (bst *T, const void *k, const void *v);
void *  bst_get                 (bst *T, const void *k);
int     bst_has                 (bst *T, const void *k);

int     bst_traverse_keys       (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_keys_r     (bst *T, int (*f)(void *k, void *p), void *p);
int     bst_traverse_values     (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_values_r   (bst *T, int (*f)(void *v, void *p), void *p);
int     bst_traverse_nodes      (bst *T, int (*f)(bstn *n, void *p), void *p);
int     bst_traverse_nodes_r    (bst *T, int (*f)(bstn *n, void *p), void *p);

#define bst_count(T) (T)->count

/* Node subroutines */

bstn *  bstn_new                (void);
void    bstn_delete             (bst *T, bstn *n);

int     bstn_set_key            (const bst *T, bstn *n, const void *k);
void *  bstn_get_key            (const bst *T, const bstn *n);
void    bstn_destroy_key        (const bst *T, bstn *n);

int     bstn_set_value          (const bst *T, bstn *n, const void *v);
void *  bstn_get_value          (const bst *T, const bstn *n);
void    bstn_destroy_value      (const bst *T, bstn *n);

void    bstn_rotate_left        (bst *T, bstn *n);
void    bstn_rotate_right       (bst *T, bstn *n);
void    bstn_replace_child      (bst *T, bstn *p, bstn *c, bstn *s);

#endif // _binary_tree_h
