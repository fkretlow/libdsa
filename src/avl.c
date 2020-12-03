/*************************************************************************************************
 *
 * avl.c
 *
 * Algorithms for insertion into and deletion from an AVL tree. The algorithms are called by the
 * high level functions of the bst interface declared in bst.h if the balancing strategy is set to
 * AVL for the tree they are called on.
 *
 * Author: Florian Kretlow, 2020
 * Use, modify, and distribute as you wish.
 *
 ************************************************************************************************/

#include <assert.h>
#include "bst.h"
#include "check.h"
#include "log.h"

#define avln_balance(n) (n)->flags.avl.balance

/* void avln_rotate_right(bstn **np, short *dhp)
 * void avln_rotate_left (bstn **np, short *dhp)
 * Normal tree rotations with updates to AVL balance factors. A change of height is reported at
 * dhp. The pointer at np is updated to hold the new root of the rotated subtree. */
void avln_rotate_right(bstn **np, short *dhp)
{
    bstn *n = *np;
    assert(n && n->left);

    bstn *p = n->left;
    short bn = avln_balance(n);
    short bp = avln_balance(p);
    assert(bn < 0 && bp >= -2 && bp <= 1);

    n->left = p->right;
    p->right = n;

    n->flags.avl.balance = bp > 0 ? bn + 1 : bn - bp + 1;
    p->flags.avl.balance = n->flags.avl.balance > 0 ? bn + 2 : bp + 1;
    *dhp = bn == -2 && bp < 0 ? -1 : 0;

    *np = p;
}

void avln_rotate_left(bstn **np, short *dhp)
{
    bstn *n = *np;
    assert(n && n->right);

    bstn *p = n->right;
    short bn = avln_balance(n);
    short bp = avln_balance(p);
    assert(bn > 0 && bp >= -1 && bp <= 2);

    n->right = p->left;
    p->left = n;

    n->flags.avl.balance = bp < 0 ? bn - 1 : bn - bp - 1;
    p->flags.avl.balance = n->flags.avl.balance < 0 ? bn - 2 : bp - 1;
    *dhp = bn == 2 && bp > 0 ? -1 : 0;

    *np = p;
}

/* void avln_repair(bstn **np, short *dhp)
 * Repair the AVL invariant after insertion/deletion on the way up the call chain. A change of
 * height is reported at dhp and the pointer at np is updated. */
void avln_repair(bstn **np, short *dhp)
{
    bstn *n = *np;
    assert(n);

    short dh  = 0;  /* change of height here */
    short dhc = 0;  /* change of height in a subtree during a subrotation */

    if (avln_balance(n) == -2) {
        assert(avln_balance(n->left) >= -1 && avln_balance(n->left) <= 1);
        if (avln_balance(n->left) == 1) {
            avln_rotate_left(&n->left, &dhc);
            n->flags.avl.balance -= dhc;
        }
        avln_rotate_right(&n, &dh);

    } else if (avln_balance(n) == 2) {
        assert(avln_balance(n->right) >= -1 && avln_balance(n->right) <= 1);
        if (avln_balance(n->right) == -1) {
            avln_rotate_right(&n->right, &dhc);
            n->flags.avl.balance += dhc;
        }
        avln_rotate_left(&n, &dh);
    }

    *dhp = dh;
    *np = n;
}

/* int avln_insert(const bst *T, bstn **np, const void *k, const void *v, int *dh)
 * Insert a node with the key k and the value v (if given) into the substree with the root n,
 * preserving the AVL invariants. A change of height is written to dhp and the pointer at np is
 * updated. Return 1 if a node was added, 0 if k was already there, or -1 on failure. */
int avln_insert(
        bst *T,
        bstn **np,
        const void *k,
        const void *v,
        short *dhp)         /* where to report a change of height */
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    bstn *n = *np;
    short dh  = 0;          /* change of height here */
    short dhr = 0;          /* change of height through repair */
    short dhc = 0;          /* change of height in the child */
    int rc;

    if (!n) {
        n = bstn_new(T, k, v);
        check(n, "failed to create new node");
        dh = 1;
        rc = 1;

    } else {
        int cmp = t_compare(T->key_type, k, bstn_key(T, n));

        if (cmp < 0) {
            rc = avln_insert(T, &n->left, k, v, &dhc);
            if (avln_balance(n) < 0 || (avln_balance(n) == 0 && dhc > 0)) dh += dhc;
            n->flags.avl.balance -= dhc;
        } else if (cmp > 0) {
            rc = avln_insert(T, &n->right, k, v, &dhc);
            if (avln_balance(n) > 0 || (avln_balance(n) == 0 && dhc > 0)) dh += dhc;
            n->flags.avl.balance += dhc;
        } else { /* cmp == 0 */
            if (v) bstn_set_value(T, n, v);
            rc = 0;
        }

        if (dhc) avln_repair(&n, &dhr);
    }

    if (dhp) *dhp = dh + dhr;
    *np = n;
    return rc;
error:
    return -1;
}

/* int avln_remove_min(const bst *T, bstn **np, short *dhp)
 * Remove the minimum from the subtree with the root n, preserving the AVL invariants. A change of
 * height is reported at dhp, and the pointer at np may be changed. This always removes a node,
 * but we return 1 whatsoever for consistency with bstn_remove. */
int avln_remove_min(bst *T, bstn **np, short *dhp)
{
    bstn *n = *np;
    assert(n);

    if (!n->left) {
        bstn *r = n->right;
        bstn_delete(T, n);
        if (dhp) *dhp = -1;
        *np = r;
        return 1;

    } else {
        short dh  = 0;          /* change of height here */
        short dhr = 0;          /* change of height through repair */
        short dhc = 0;          /* change of height in the child */

        int rc = avln_remove_min(T, &n->left, &dhc);
        if (avln_balance(n) < 0) dh += dhc;
        n->flags.avl.balance -= dhc;

        if (dhc) avln_repair(&n, &dhr);
        if (dhp) *dhp = dh + dhr;
        *np = n;
        return rc;
    }
}

/* int avln_remove(const bst *T, bstn **np, const void *k, short *dhp)
 * Remove the node with the key k from the substree roted at n, preserving the AVL invariant. A
 * change of height is written to dhp, and the pointer at np may be changed. Return 1 if a node
 * was removed, 0 if k wasn't found, or -1 on error. */
int avln_remove(bst *T, bstn **np, const void *k, short *dhp)
{
    assert(T && T->key_type && k);

    bstn *n = *np;
    if (!n) return 0;

    short dh  = 0;          /* change of height here */
    short dhr = 0;          /* change of height through repair */
    short dhc = 0;          /* change of height in the child */
    int rc;

    int cmp = t_compare(T->key_type, k, bstn_key(T, n));

    if (cmp < 0) {
        rc = avln_remove(T, &n->left, k, &dhc);
        if (avln_balance(n) < 0) dh += dhc;
        n->flags.avl.balance -= dhc;

    } else if (cmp > 0) {
        rc = avln_remove(T, &n->right, k, &dhc);
        if (avln_balance(n) > 0) dh += dhc;
        n->flags.avl.balance += dhc;

    } else { /* cmp == 0 */
        if (n->left && n->right) {
            /* Find the node with the minimum key in the right subtree, which is guaranteed to not
             * have a left child; move its data over here, then delete it. */
            bstn *s = n->right;
            while (s->left) s = s->left;
            bstn_move_data(T, n, s);
            rc = avln_remove_min(T, &n->right, &dhc);
            if (avln_balance(n) > 0) dh += dhc;
            n->flags.avl.balance += dhc;

        } else {
            /* use np to temporarily store the successor */
            if      (n->left)   *np = n->left;
            else if (n->right)  *np = n->right;
            else                *np = NULL;

            bstn_delete(T, n);
            n = *np;
            dh = -1;
            rc = 1;
        }
    }

    if (dhc) avln_repair(&n, &dhr);
    if (dhp) *dhp = dh + dhr;
    *np = n;
    return rc;
}

/* int avln_invariant(const bst *T, const bstn *n, int depth, struct bst_stats *s)
 * Check if the subtree with the root n satisfies the inequality properties for keys in BSTs and
 * the AVL properties, and collect stats of the tree while at it. */
int avln_invariant(const bst *T, const bstn *n, int depth, int *h_out, struct bst_stats *s)
{
    if (!n) return 0;

    ++depth;
    ++s->total_nodes;

    if (!n->left && !n->right) {
        if (!s->shortest_path || depth < s->shortest_path) s->shortest_path = depth;
        if (!s->height        || depth > s->height)        s->height = depth;
    }

    /* check key inequalities */
    if (n->left && t_compare(T->key_type, bstn_key(T, n->left), bstn_key(T, n)) >= 0) {
        log_error("BST invariant violated: left child > parent");
        return -1;
    }
    if (n->right && t_compare(T->key_type, bstn_key(T, n->right), bstn_key(T, n)) <= 0) {
        log_error("BST invariant violated: right child < parent");
        return -1;
    }

    /* process children first to get the height of the subtrees */
    int rc;
    int hl = 0;
    int hr = 0;
    if (n->left) {
        rc = avln_invariant(T, n->left, depth, &hl, s);
        if (rc != 0) return rc;
    }
    if (n->right) {
        rc = avln_invariant(T, n->right, depth, &hr, s);
        if (rc != 0) return rc;
    }
    if (h_out) *h_out = (hl > hr ? hl : hr) + 1;

    /* check balance */
    if (avln_balance(n) != hr - hl) {
        log_error("AVL balance factor is wrong: b = %d != %lu - %lu", avln_balance(n), hr, hl);
        return -2;
    }

    if (avln_balance(n) < -1 || avln_balance(n) > 1) {
        log_error("AVL invariant violated: out of balance");
        return -3;
    }

    return 0;
}
