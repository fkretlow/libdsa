#include <assert.h>
#include "bst.h"
#include "check.h"
#include "log.h"

#define avln_balance(n) (n)->flags.avl.balance

/* void avln_rotate_right(bstn **np, short *dhp)
 * void avln_rotate_left (bstn **np, short *dhp)
 * Normal tree rotations with updates to AVL balance factors. A change of height is reported at
 * dhp. */
void avln_rotate_right(bstn **np, short *dhp)
{
    bstn *n = *np;
    assert(n && n->left);

    bstn *p = n->left;
    short bn = avln_balance(n);
    short bp = avln_balance(p);
    assert(bn < 0 && bp >= -1 && bp <= 1);

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
    assert(bn > 0 && bp >= -1 && bp <= 1);

    n->right = p->left;
    p->left = n;

    n->flags.avl.balance = bp < 0 ? bn - 1 : bn - bp - 1;
    p->flags.avl.balance = n->flags.avl.balance < 0 ? bn - 2 : bp - 1;
    *dhp = bn == 2 && bp > 0 ? -1 : 0;

    *np = p;
}

/* void avln_repair(bstn **np, short *dhp)
 * Repair the AVL invariant after insertion/deletion on the way up the call chain. A change of
 * height is reported at dhp. */
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
 * preserving the AVL invariants.  The pointer at np may be changed. Return 1 if a node was added,
 * 0 if k was already there, or -1 on failure. */
int avln_insert(
        bst *T,
        bstn **np,
        const void *k,
        const void *v,
        short *dhp)        /* where to report a change of height */
{
    assert(T && T->key_type && k);
    assert(!v || T->value_type);

    bstn *n = *np;
    short dhc;          /* change of height in the child */
    short dh;           /* change of height here */
    int rc;

    if (!n) {
        n = bstn_new(T, k, v);
        check(n, "failed to create new node");
        *np = n;
        *dhp = 1;
        return 1;
    }

    int cmp = t_compare(T->key_type, k, bstn_key(T, n));

    if (cmp < 0) {
        rc = avln_insert(T, &n->left, k, v, &dhc);
        n->flags.avl.balance -= dhc;
    } else if (cmp > 0) {
        rc = avln_insert(T, &n->right, k, v, &dhc);
        n->flags.avl.balance += dhc;
    } else { /* cmp == 0 */
        if (v) bstn_set_value(T, n, v);
        rc = 0;
    }

    avln_repair(&n, &dh);
    *dhp = dh;
    *np = n;
    return rc;
error:
    return -1;
}
