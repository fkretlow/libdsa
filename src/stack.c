#include <assert.h>

#include "check.h"
#include "stack.h"

static inline int stack_invariant(const stack *S)
{
    if (S->top) check(S->count > 0, "stack invariant violated: S->top && S->count == 0");
    return 0;
error:
    return -1;
}

static inline stackn *stackn_new(void)
{
    stackn *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void stackn_delete(const stack *S, stackn *n)
{
    if (n) {
        if (n->data && S) {
            t_destroy(S->data_type, n->data);
        }
    }
    free(n->data);
    free(n);
}

static int stackn_set(const stack *S, stackn *n, const void *in)
{
    check_ptr(S);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        t_destroy(S->data_type, n->data);
    } else {
        n->data = t_allocate(S->data_type, 1);
        check(n->data != NULL, "Failed to allocate memory for new element.");
    }

    t_copy(S->data_type, n->data, in);

    return 0;
error:
    return -1;
}

static inline int stackn_get(const stack *S, stackn *n, void *out)
{
    check_ptr(S);
    check_ptr(n);
    check_ptr(out);

    t_copy(S->data_type, out, n->data);

    return 0;
error:
    return -1;
}

int stack_initialize(stack *S, t_intf *dt)
{
    check_ptr(dt);

    S->top = NULL;
    S->count = 0;
    S->data_type = dt;

    return 0;
error:
    return -1;
}

stack *stack_new(t_intf *dt)
{
    stack *S = malloc(sizeof(*S));
    check_alloc(S);

    int rc = stack_initialize(S, dt);
    check(rc == 0, "Failed to initialize stack.");
    assert(!stack_invariant(S));

    return S;
error:
    if (S) free(S);
    return NULL;
}

void stack_delete(stack *S)
{
    if (S) {
        if (S->count) stack_clear(S);
        free(S);
    }
}

void stack_clear(stack *S)
{
    assert(!stack_invariant(S));

    stackn *cur;
    stackn *next;

    for (cur = S->top; cur != NULL; cur = next) {
        next = cur->next;
        stackn_delete(S, cur);
    }
    S->top = NULL;
    S->count = 0;
}

int stack_push(stack *S, const void *in)
{
    check_ptr(S);
    assert(!stack_invariant(S));
    check_ptr(in);

    stackn *n = stackn_new();
    check(n != NULL, "Failed to make new node.");
    check(!stackn_set(S, n, in), "Failed to write data to new node.");

    n->next = S->top;
    S->top = n;
    ++S->count;

    assert(!stack_invariant(S));
    return 0;
error:
    return -1;
}

int stack_pop(stack *S, void *out)
{
    check_ptr(S);
    assert(!stack_invariant(S));
    check_ptr(out);
    check(S->count > 0, "Attempt to pop from empty stack.");

    stackn *n = S->top;

    check(!stackn_get(S, n, out), "Failed to hand out value.");

    S->top = n->next;
    --S->count;

    stackn_delete(S, n);

    assert(!stack_invariant(S));
    return 0;
error:
    return -1;
}
