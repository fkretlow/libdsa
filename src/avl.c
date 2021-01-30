/*************************************************************************************************
 *
 * avl.c
 *
 * Algorithms for insertion into and deletion from an AVL tree. The algorithms are called by the
 * high level functions of the bst interface declared in bst.h if the balancing strategy is set to
 * AVL for the tree they are called on. See the docstring in bst.c for additional information.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#include <assert.h>
#include "bst.h"
#include "check.h"
#include "log.h"

#define avl_n_balance(n) (n)->flags.avl.balance

/* void avl_n_rotate_right(bst_n **np, short *dhp)
 * void avl_n_rotate_left (bst_n **np, short *dhp)
 * Normal tree rotations with updates to AVL balance factors. A change of height is reported at
 * dhp. The pointer at np is updated to hold the new root of the rotated subtree. */
void avl_n_rotate_right(bst_n **np, short *dhp)
{
    bst_n *n = *np;
    assert(n && n->left);

    bst_n *p = n->left;
    short bn = avl_n_balance(n);
    short bp = avl_n_balance(p);
    assert(bn < 0 && bp >= -2 && bp <= 1);

    n->left = p->right;
    p->right = n;

    n->flags.avl.balance = bp > 0 ? bn + 1 : bn - bp + 1;
    p->flags.avl.balance = n->flags.avl.balance > 0 ? bn + 2 : bp + 1;
    *dhp = bn == -2 && bp < 0 ? -1 : 0;

    *np = p;
}

void avl_n_rotate_left(bst_n **np, short *dhp)
{
    bst_n *n = *np;
    assert(n && n->right);

    bst_n *p = n->right;
    short bn = avl_n_balance(n);
    short bp = avl_n_balance(p);
    assert(bn > 0 && bp >= -1 && bp <= 2);

    n->right = p->left;
    p->left = n;

    n->flags.avl.balance = bp < 0 ? bn - 1 : bn - bp - 1;
    p->flags.avl.balance = n->flags.avl.balance < 0 ? bn - 2 : bp - 1;
    *dhp = bn == 2 && bp > 0 ? -1 : 0;

    *np = p;
}

/* void avl_n_repair(bst_n **np, short *dhp)
 * Repair the AVL invariant after insertion/deletion on the way up the call chain. A change of
 * height is reported at dhp and the pointer at np is updated. */
void avl_n_repair(bst_n **np, short *dhp)
{
    bst_n *n = *np;
    assert(n);

    short dh  = 0;  /* change of height here */
    short dhc = 0;  /* change of height in a subtree during a subrotation */

    if (avl_n_balance(n) == -2) {
        assert(avl_n_balance(n->left) >= -1 && avl_n_balance(n->left) <= 1);
        if (avl_n_balance(n->left) == 1) {
            avl_n_rotate_left(&n->left, &dhc);
            n->flags.avl.balance -= dhc;
        }
        avl_n_rotate_right(&n, &dh);

    } else if (avl_n_balance(n) == 2) {
        assert(avl_n_balance(n->right) >= -1 && avl_n_balance(n->right) <= 1);
        if (avl_n_balance(n->right) == -1) {
            avl_n_rotate_right(&n->right, &dhc);
            n->flags.avl.balance += dhc;
        }
        avl_n_rotate_left(&n, &dh);
    }

    *dhp = dh;
    *np = n;
}

/* int avl_n_insert(const bst *T, bst_n **np, const void *k, const void *v, int *dh)
 * Insert a node with the key k and the value v (if given) into the substree with the root n,
 * preserving the AVL invariants. A change of height is written to dhp and the pointer at np is
 * updated. Return 1 if a node was added, 0 if k was already there, or -1 on failure. */
int avl_n_insert(
        bst *T,
        bst_n **np,
        const void *k,
        const void *v,
        short *dhp)         /* where to report a change of height */
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    bst_n *n = *np;
    short dh  = 0;          /* change of height here */
    short dhr = 0;          /* change of height through repair */
    short dhc = 0;          /* change of height in the child */
    int rc;

    if (!n) {
        n = bst_n_new(T, k, v);
        check(n, "failed to create new node");
        dh = 1;
        rc = 1;

    } else {
        int cmp = t_compare(T->key_type, k, bst_n_key(T, n));

        if (cmp < 0) {
            rc = avl_n_insert(T, &n->left, k, v, &dhc);
            if (avl_n_balance(n) < 0 || (avl_n_balance(n) == 0 && dhc > 0)) dh += dhc;
            n->flags.avl.balance -= dhc;
        } else if (cmp > 0) {
            rc = avl_n_insert(T, &n->right, k, v, &dhc);
            if (avl_n_balance(n) > 0 || (avl_n_balance(n) == 0 && dhc > 0)) dh += dhc;
            n->flags.avl.balance += dhc;
        } else { /* cmp == 0 */
            if (v) bst_n_set_value(T, n, v);
            rc = 0;
        }

        if (dhc) avl_n_repair(&n, &dhr);
    }

    if (dhp) *dhp = dh + dhr;
    *np = n;
    return rc;
error:
    return -1;
}

/* int avl_n_remove_min(const bst *T, bst_n **np, short *dhp)
 * Remove the minimum from the subtree with the root n, preserving the AVL invariants. A change of
 * height is reported at dhp, and the pointer at np may be changed. This always removes a node,
 * but we return 1 whatsoever for consistency with bst_n_remove. */
int avl_n_remove_min(bst *T, bst_n **np, short *dhp)
{
    bst_n *n = *np;
    assert(n);

    if (!n->left) {
        bst_n *r = n->right;
        bst_n_delete(T, n);
        if (dhp) *dhp = -1;
        *np = r;
        return 1;

    } else {
        short dh  = 0;          /* change of height here */
        short dhr = 0;          /* change of height through repair */
        short dhc = 0;          /* change of height in the child */

        int rc = avl_n_remove_min(T, &n->left, &dhc);
        if (avl_n_balance(n) < 0) dh += dhc;
        n->flags.avl.balance -= dhc;

        if (dhc) avl_n_repair(&n, &dhr);
        if (dhp) *dhp = dh + dhr;
        *np = n;
        return rc;
    }
}

/* int avl_n_remove(const bst *T, bst_n **np, const void *k, short *dhp)
 * Remove the node with the key k from the substree roted at n, preserving the AVL invariant. A
 * change of height is written to dhp, and the pointer at np may be changed. Return 1 if a node
 * was removed, 0 if k wasn't found, or -1 on error. */
int avl_n_remove(bst *T, bst_n **np, const void *k, short *dhp)
{
    assert(T && T->key_type && k);

    bst_n *n = *np;
    if (!n) return 0;

    short dh  = 0;          /* change of height here */
    short dhr = 0;          /* change of height through repair */
    short dhc = 0;          /* change of height in the child */
    int rc;

    int cmp = t_compare(T->key_type, k, bst_n_key(T, n));

    if (cmp < 0) {
        rc = avl_n_remove(T, &n->left, k, &dhc);
        if (avl_n_balance(n) < 0) dh += dhc;
        n->flags.avl.balance -= dhc;

    } else if (cmp > 0) {
        rc = avl_n_remove(T, &n->right, k, &dhc);
        if (avl_n_balance(n) > 0) dh += dhc;
        n->flags.avl.balance += dhc;

    } else { /* cmp == 0 */
        if (n->left && n->right) {
            /* Find the node with the minimum key in the right subtree, which is guaranteed to not
             * have a left child; move its data over here, then delete it. */
            bst_n *s = n->right;
            while (s->left) s = s->left;
            bst_n_move_data(T, n, s);
            rc = avl_n_remove_min(T, &n->right, &dhc);
            if (avl_n_balance(n) > 0) dh += dhc;
            n->flags.avl.balance += dhc;

        } else {
            /* use np to temporarily store the successor */
            if      (n->left)   *np = n->left;
            else if (n->right)  *np = n->right;
            else                *np = NULL;

            bst_n_delete(T, n);
            n = *np;
            dh = -1;
            rc = 1;
        }
    }

    if (dhc) avl_n_repair(&n, &dhr);
    if (dhp) *dhp = dh + dhr;
    *np = n;
    return rc;
}

/* int avl_n_invariant(const bst *T, const bst_n *n, int depth, struct bst_stats *s)
 * Check if the subtree with the root n satisfies the inequality properties for keys in BSTs and
 * the AVL properties, and collect stats of the tree while at it. The height of the subtree with
 * the root n is reported at h_out so the balance factors of the ancestors can be checked. */
int avl_n_invariant(const bst *T, const bst_n *n, int depth, int *h_out, struct bst_stats *s)
{
    if (!n) return 0;

    ++depth;
    ++s->total_nodes;

    if (!n->left && !n->right) {
        if (!s->shortest_path || depth < s->shortest_path) s->shortest_path = depth;
        if (!s->height        || depth > s->height)        s->height = depth;
    }

    /* check key inequalities */
    if (n->left && t_compare(T->key_type, bst_n_key(T, n->left), bst_n_key(T, n)) >= 0) {
        log_error("BST invariant violated: left child > parent");
        return -1;
    }
    if (n->right && t_compare(T->key_type, bst_n_key(T, n->right), bst_n_key(T, n)) <= 0) {
        log_error("BST invariant violated: right child < parent");
        return -1;
    }

    /* process children first to get the height of the subtrees */
    int rc;
    int hl = 0;
    int hr = 0;
    if (n->left) {
        rc = avl_n_invariant(T, n->left, depth, &hl, s);
        if (rc != 0) return rc;
    }
    if (n->right) {
        rc = avl_n_invariant(T, n->right, depth, &hr, s);
        if (rc != 0) return rc;
    }
    if (h_out) *h_out = (hl > hr ? hl : hr) + 1;

    /* check balance */
    if (avl_n_balance(n) != hr - hl) {
        log_error("AVL balance factor is wrong: b = %d != %lu - %lu", avl_n_balance(n), hr, hl);
        return -2;
    }

    if (avl_n_balance(n) < -1 || avl_n_balance(n) > 1) {
        log_error("AVL invariant violated: out of balance");
        return -3;
    }

    return 0;
}
