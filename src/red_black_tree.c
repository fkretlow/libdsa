#include <assert.h>

#include "check.h"
#include "binary_tree.h"

/* Return the weight of the group that contains n. n can be any node in the group. */
static inline unsigned short rbt_group_weight(btn *n)
{
    if (!n) return 0;
    if (rbtn_color(n) == RED) n = n->parent;
    unsigned short w = 1;
    if (n->left && rbtn_color(n->left) == RED) ++w;
    if (n->right && rbtn_color(n->right) == RED) ++w;
    return w;
}

static void rbt_group_decrease_weight(bt *T, btn *n)
{
    btn *p = n->parent;
    btn *pp = p ? p->parent : NULL;
    btn *l = n->left;
    btn *r = n->right;

    assert(rbt_group_weight(n) == 3);

    /* Case 1: n is the root of the tree. */
    if (n == T->root) {
        /* log_debug("Case 1: n is the root"); */
        rbtn_set_color(l, BLACK);
        rbtn_set_color(r, BLACK);
    }

    /* Case 2: p is black. */
    else if (rbtn_color(p) == BLACK) {
        /* log_debug("Case 2: p is black"); */
        rbtn_set_color(n, RED);
        rbtn_set_color(l, BLACK);
        rbtn_set_color(r, BLACK);
    }

    /* Case 3: p is red. */
    else if (rbtn_color(p) == RED) {

        /* Case 3.1: pp has 1 red child (p) */
        btn *ps = p == pp->left ? pp->right : pp->left;
        assert(ps);
        if (rbtn_color(ps) == BLACK) {

            /* Case 3.1a: left-left or right-right ancestor chain. */
            if (n == pp->right->right || n == pp->left->left) {
                /* log_debug("Case 3.1a straight ancestor chain"); */
                if (n == pp->right->right) {
                    btn_rotate_left(T, pp, NULL);
                } else {
                    btn_rotate_right(T, pp, NULL);
                }
                rbtn_set_color(pp, RED);
                rbtn_set_color(p, BLACK);
                rbtn_set_color(n, RED);
                rbtn_set_color(l, BLACK);
                rbtn_set_color(r, BLACK);
            }

            /* Case 3.1b: right-left or left-right ancestor chain. */
            else {
                /* log_debug("Case 3.1b: right-left or left-right ancestor chain"); */
                if (n == pp->right->left) {
                    btn_rotate_right(T, p, NULL);
                    btn_rotate_left(T, pp, NULL);
                } else {
                    btn_rotate_left(T, p, NULL);
                    btn_rotate_right(T, pp, NULL);
                }
                rbtn_set_color(pp, RED);
                rbtn_set_color(l, BLACK);
                rbtn_set_color(r, BLACK);
            }
        }

        /* Case 3.2: pp has 2 red children */
        else {
            /* log_debug("Case 3.2: pp is full."); */
            rbt_group_decrease_weight(T, pp);
            /* Things may have changed. Go again. */
            rbt_group_decrease_weight(T, n);
        }
    }
}

/* int rbtn_insert(const bt *T, btn *n, const void *k, btn **n_out)
 * Insert a node with the key k into the subtree rooted at n, preserving the red-black tree
 * invariants. A pointer to the new node is saved in n_out. Returns 1 if a node was added, 0 if k
 * was already there, and -1 on error. */

int rbtn_insert(
        bt *T,
        btn *n,         /* the root of the subtree, NULL if the tree is empty */
        const void *k,  /* the key to insert */
        btn **n_out)    /* pass back a pointer to the new node, can be NULL */
{
    log_call("T=%p, n=%p, k=%p, n_out=%p", T, n, k, n_out);
    assert(T && T->key_type && k);

    if (n == NULL) {
        /* log_debug("inserting a root"); */
        assert(T->count == 0);
        T->root = btn_new(T);
        T->root->parent = NULL;
        rbtn_set_color(T->root, BLACK);
        btn_set_key(T, T->root, k);
        if (n_out) *n_out = T->root;
        return 1;
    }

    int comp = t_compare(T->key_type, k, btn_key(T, n));
    if (comp == 0) {
        if (n_out) *n_out = n;
        return 0; /* found it, nothing to do */
    } else if (comp < 0 && n->left) {
        return rbtn_insert(T, n->left, k, n_out);
    } else if (comp > 0 && n->right) {
        return rbtn_insert(T, n->right, k, n_out);
    }

    /* Case 1: n is black */
    if (rbtn_color(n) == BLACK) {
        if (comp < 0) {
            /* log_debug("Case 1 left"); */
            n->left = btn_new(T);
            assert(!btn_has_key(n->left));
            btn_set_key(T, n->left, k);
            n->left->parent = n;
            rbtn_set_color(n->left, RED);
            if (n_out) *n_out = n->left;
        } else { /* comp > 0 */
            /* log_debug("Case 1 right"); */
            n->right = btn_new(T);
            assert(!btn_has_key(n->right));
            btn_set_key(T, n->right, k);
            n->right->parent = n;
            rbtn_set_color(n->right, RED);
            if (n_out) *n_out = n->right;
        }
        return 1;
    }

    /* Case 2: n is red */
    else {
        btn *p = n->parent;

        /* Case 2.1: p has 2 red children */
        if (rbt_group_weight(p) == 3) {
            /* log_debug("Case 2.1 p is full"); */
            rbt_group_decrease_weight(T, p);
            /* Now n may be elsewhere. Go again. */
            return rbtn_insert(T, n, k, n_out);
        }

        /* Case 2.2: p has 1 red child (n) */
        else { /* weight(p) = 1 is handled in case 1 */
            if (comp < 0 && n == p->left) {
                /* log_debug("Case 2.2 left-left"); */
                btn_rotate_right(T, p, NULL);
                n->left = btn_new(T);
                n->left->parent = n;
                btn_set_key(T, n->left, k);
                rbtn_set_color(n, BLACK);
                rbtn_set_color(p, RED);
                rbtn_set_color(n->left, RED);
                if (n_out) *n_out = n->left;

            } else if (comp > 0 && n == p->right) {
                /* log_debug("Case 2.2 right-right"); */
                btn_rotate_left(T, p, NULL);
                n->right = btn_new(T);
                n->right->parent = n;
                btn_set_key(T, n->right, k);
                rbtn_set_color(n, BLACK);
                rbtn_set_color(p, RED);
                rbtn_set_color(n->right, RED);
                if (n_out) *n_out = n->right;

            } else if (comp < 0 && n == p->right) {
                /* log_debug("Case 2.2 right-left"); */
                p->left = btn_new(T);
                p->left->parent = p;
                rbtn_set_color(p->left, RED);

                t_move(T->key_type, btn_key(T, p->left), btn_key(T, p));
                p->flags.rb.has_key = 0;
                p->left->flags.rb.has_key = 1;
                if (p->flags.rb.has_value) {
                    t_move(T->value_type, btn_value(T, p->left), btn_value(T, p));
                    p->flags.rb.has_value = 0;
                    p->left->flags.rb.has_value = 1;
                }
                btn_set_key(T, p, k);

                if (n_out) *n_out = p;

            } else if (comp > 0 && n == p->left) {
                /* log_debug("Case 2.2 left-right"); */
                p->right = btn_new(T);
                p->right->parent = p;
                rbtn_set_color(p->right, RED);

                t_move(T->key_type, btn_key(T, p->right), btn_key(T, p));
                p->flags.rb.has_key = 0;
                p->right->flags.rb.has_key = 1;
                if (p->flags.rb.has_value) {
                    t_move(T->value_type, btn_value(T, p->right), btn_value(T, p));
                    p->flags.rb.has_value = 0;
                    p->right->flags.rb.has_value = 1;
                }
                btn_set_key(T, p, k);

                if (n_out) *n_out = p;
            }
            return 1;
        }
    }
}

/* int rbt_insert(bt *T, const void *k)
 * Insert k into the tree, preserving the red-black tree properties. Return 0 or 1 depending on
 * whether a node was added or k was already there, or -1 on error. */

int rbt_insert(bt *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(rbt_invariant(T) == 0);

    int rc = rbtn_insert(T, T->root, k, NULL);
    if (rc == 1) ++T->count;

    assert(rbt_invariant(T) == 0);
    return rc;
error:
    return -1;
}

/***************************************************************************************
 *
 * static void rbt_group_increase_weight(bt *T, btn *n)
 *
 * Increase the weight of the group with the head n. It is assumed that n is the head of
 * an empty group, i.o.w. rbtn_color(n) == BLACK && !n->left && !n->right.  After the
 * function exits n can be both red or black.
 *
 * See ./doc/red_black_tree.pdf for a description of the algorithm.
 *
 **************************************************************************************/

static void rbt_group_increase_weight(bt *T, btn *n)
{
    btn *p = n->parent;
    btn *s = p ? (n == p->left ? p->right : p->left) : NULL;
    btn *so = NULL; /* outer child of s */

    /* Case 1: ancestor group is not empty. */
    if (rbt_group_weight(p) > 1) {
        if (rbtn_color(p) == BLACK) {
            if (n == p->right) {
                btn_rotate_right(T, p, NULL);
                s = p->left;
            } else {
                btn_rotate_left(T, p, NULL);
                s = p->right;
            }
            rbtn_set_color(p->parent, BLACK);
            rbtn_set_color(p, RED);
        }

        assert(rbtn_color(p) == RED);

        /* Case 1.1: s is empty. */
        if (rbt_group_weight(s) == 1) {
            rbtn_set_color(p, BLACK);
            rbtn_set_color(n, RED);
            rbtn_set_color(s, RED);
        }

        /* Case 1.2: s is not empty. */
        else {
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || rbtn_color(s->left) == BLACK)) {
                btn_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                rbtn_set_color(s, BLACK);
                rbtn_set_color(so, RED);
            } else if (s == p->right && (!s->right || rbtn_color(s->right) == BLACK)) {
                btn_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                rbtn_set_color(s, BLACK);
                rbtn_set_color(so, RED);
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                btn_rotate_right(T, p, NULL);
            } else {
                btn_rotate_left(T, p, NULL);
            }

            rbtn_set_color(p, BLACK);
            rbtn_set_color(n, RED);
            rbtn_set_color(s, RED);
            rbtn_set_color(so, BLACK);
        }
    }

    /* Case 2: ancestor group is empty. */
    else { /* rbt_group_weight(p) <= 1 */
        assert(s); /* TODO: why do we not have an s? */
        /* Case 2.1: s is empty. */
        if (rbt_group_weight(s) == 1) {
            if (p == T->root) {
                rbtn_set_color(n, RED);
                rbtn_set_color(s, RED);
            } else {
                rbt_group_increase_weight(T, p);
                return rbt_group_increase_weight(T, n);
            }
        }

        /* Case 2.2: s is not empty. Like 1.2 with different color changes. */
        else { /* rbt_group_weight(s) > 1 */
            /* Ensure that s has an outer child. */
            if (s == p->left && (!s->left || rbtn_color(s->left) == BLACK)) {
                btn_rotate_left(T, s, &s); /* Note that s is updated. */
                so = s->left;
                rbtn_set_color(s, BLACK);
                rbtn_set_color(so, RED);
            } else if (s == p->right && (!s->right || rbtn_color(s->right) == BLACK)) {
                btn_rotate_right(T, s, &s); /* Note that s is updated. */
                so = s->right;
                rbtn_set_color(s, BLACK);
                rbtn_set_color(so, RED);
            } else {
                so = n == p->right ? s->left : s->right;
            }

            if (n == p->right) {
                btn_rotate_right(T, p, NULL);
            } else {
                btn_rotate_left(T, p, NULL);
            }

            rbtn_set_color(n, RED);
            rbtn_set_color(so, BLACK);
        }
    }
}

/* int rbtn_remove(const bt *T, btn *n, const void *k)
 * Remove the node with the key k from the subtree roted at n, preserving the red-black tree
 * properties. Returns 1 if a node was deleted, 0 if k wasn't found, and -1 on error. */

int rbtn_remove(
        bt *T,
        btn *n,         /* the root of the subtree to delete from */
        const void *k)  /* the key to delete */
{
    log_call("T=%p, n=%p, k=%p", T, n, k);
    assert(T && T->key_type && n && k);

    int comp;
    for ( ;; ) {
        if (!n) return 0; /* not found */
        comp = t_compare(T->key_type, k, btn_key(T, n));
        if (comp == 0) break; /* found, go on */
        else if (comp < 0) n = n->left;
        else if (comp > 0) n = n->right;
    }

    if (n->left && n->right) {
        /* find the node with the next greater key and swap */
        btn *s = n->right;
        while (s->left) s = s->left;

        /* destroy the data in n */
        btn_destroy_key(T, n);
        if (btn_has_value(n)) btn_destroy_value(T, n);

        /* move over the data from s */
        t_move(T->key_type, btn_key(T, n), btn_key(T, s));
        s->flags.plain.has_key = 0;
        n->flags.plain.has_key = 1;

        if (btn_has_value(s)) {
            t_move(T->value_type, btn_value(T, n), btn_value(T, s));
            s->flags.plain.has_value = 0;
            n->flags.plain.has_value = 1;
        }

        /* move on, delete s, now stored in n */
        n = s;
    }

    assert(!n->right || !n->left);

    if (rbtn_color(n) == BLACK && n != T->root && rbt_group_weight(n) == 1) {
        rbt_group_increase_weight(T, n);
        assert(rbt_group_weight(n) > 1);
    }

    if (rbtn_color(n) == BLACK) {
        if (n->left) {
            btn_rotate_right(T, n, NULL);
            rbtn_set_color(n, RED);
            rbtn_set_color(n->parent, BLACK);
        } else if (n->right) {
            btn_rotate_left(T, n, NULL);
            rbtn_set_color(n, RED);
            rbtn_set_color(n->parent, BLACK);
        }
    }

    if (n == T->root) {
        T->root = NULL;
    } else if (n == n->parent->left) {
        n->parent->left = NULL;
    } else {
        n->parent->right = NULL;
    }
    btn_delete(T, n);

    return 1; /* found it, deleted it */
}

/* int rbt_remove(bt *T, const void *k)
 * Remove k from the tree, preserving the red-black tree invariants. Return 0 or 1 depending on
 * whether k was found and removed or not, or 1 on error. */

int rbt_remove(bt *T, const void *k)
{
    log_call("T=%p, k=%p", T, k);
    check_ptr(T);
    check_ptr(k);
    check(T->key_type, "no key type defined");
    assert(rbt_invariant(T) == 0);

    int rc = T->root ? rbtn_remove(T, T->root, k) : 0;
    if (rc == 1) --T->count;

    assert(rbt_invariant(T) == 0);
    return rc;
error:
    return -1;

}

/* int rbtn_invariant(const bt *T, const btn *n)
 * int rbt_invariant(bt *T)
 * Check if T satisfies the inequality properties for keys in binary trees and the red-black tree
 * properties. */

int rbtn_invariant(const bt *T, btn *n, int *black_count)
{
    if (n) {
        int rc;

        if (n->left) {
            rc = rbtn_invariant(T, n->left, black_count);
            if (rc != 0) return rc;
        }

        /* valid binary tree? */
        if (n->parent) {
            if (n == n->parent->left) {
                if (t_compare(T->key_type, btn_key(T, n), btn_key(T, n->parent)) > 0) {
                    log_error("binary tree invariant violated: left child > parent");
                    return -1;
                }
            } else { /* n == n->parent->right */
                if (t_compare(T->key_type, btn_key(T, n), btn_key(T, n->parent)) < 0) {
                    log_error("binary tree invariant violated: right child < parent");
                    return -1;
                }
            }
        }

        /* adjacent red nodes */
        if (rbtn_color(n) == RED) {
            if (n->parent && rbtn_color(n->parent) == RED) {
                log_error("red-black tree invariant violated: two adjacent red nodes");
                return -2;
            }
        }

        /* leaf position? if so, correct number of black nodes the path to the root? */
        if (!n->left || !n->right) {
            int count = 0;
            btn *m = n;
            while (m != NULL) {
                if (rbtn_color(m) == BLACK) ++count;
                m = m->parent;
            }
            if (*(int*)black_count == -1) {
                *(int*)black_count = count;
            } else if (count != *(int*)black_count) {
                log_error("red-black tree invariant violated: bad number of groups on path");
                return -3;
            }
        }

        if (n->right) {
            rc = rbtn_invariant(T, n->right, black_count);
            if (rc != 0) return rc;
        }
    }
    return 0;
}

int rbt_invariant(const bt *T)
{
    int black_count = -1;
    int rc = rbtn_invariant(T, T->root, &black_count);
    return rc;
}
