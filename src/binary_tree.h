#ifndef _binary_tree_h
#define _binary_tree_h

#include <stdint.h>

#include "type_interface.h"

struct btn_flags {
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

struct btn;
typedef struct btn {
    struct btn *parent;
    struct btn *left;
    struct btn *right;
    union {
        struct btn_flags  plain;
        struct rbtn_flags  red_black;
        struct avltn_flags avl;
    } flags;
} btn;

#define NONE 0
#define RED_BLACK 1
#define AVL 2

typedef struct {
    btn *root;
    uint32_t count;
    uint8_t flavor;
    t_intf *key_type;
    t_intf *value_type;
} bt;

/* Interface */

int     bt_initialize          (bt *T, uint8_t flavor, t_intf *kt, t_intf *vt);
bt *    bt_new                 (       uint8_t flavor, t_intf *kt, t_intf *vt);
void    bt_destroy             (bt *T);
void    bt_delete              (bt *T);

int     bt_copy                (bt *dest, bt *src);
void    bt_clear               (bt *T);

int     bt_insert              (bt *T, const void *k);
int     bt_remove              (bt *T, const void *k);
int     bt_set                 (bt *T, const void *k, const void *v);
void *  bt_get                 (bt *T, const void *k);
int     bt_has                 (bt *T, const void *k);

int     bt_traverse_keys       (bt *T, int (*f)(void *k, void *p), void *p);
int     bt_traverse_keys_r     (bt *T, int (*f)(void *k, void *p), void *p);
int     bt_traverse_values     (bt *T, int (*f)(void *v, void *p), void *p);
int     bt_traverse_values_r   (bt *T, int (*f)(void *v, void *p), void *p);
int     bt_traverse_nodes      (bt *T, int (*f)(btn *n, void *p), void *p);
int     bt_traverse_nodes_r    (bt *T, int (*f)(btn *n, void *p), void *p);

#define bt_count(T) (T)->count

/* Node subroutines */

btn *   btn_new                (void);
void    btn_delete             (bt *T, btn *n);

int     btn_set_key            (const bt *T, btn *n, const void *k);
void *  btn_get_key            (const bt *T, const btn *n);
void    btn_destroy_key        (const bt *T, btn *n);

int     btn_set_value          (const bt *T, btn *n, const void *v);
void *  btn_get_value          (const bt *T, const btn *n);
void    btn_destroy_value      (const bt *T, btn *n);

void    btn_rotate_left        (bt *T, btn *n);
void    btn_rotate_right       (bt *T, btn *n);
void    btn_replace_child      (bt *T, btn *p, btn *c, btn *s);

#endif // _binary_tree_h
