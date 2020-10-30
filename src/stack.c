#include <assert.h>

#include "debug.h"
#include "stack.h"

static inline int _stack_invariant(const Stack S)
{
    if (S->top) check(S->size > 0, "Stack invariant violated: S->top && S->size == 0");
    return 0;
error:
    return -1;
}

static inline _stack_node *_stack_node_new(void)
{
    _stack_node *n = calloc(1, sizeof(*n));
    check_alloc(n);
    return n;
error:
    return NULL;
}

static inline void _stack_node_delete(const Stack S, _stack_node *n)
{
    if (n) {
        if (n->data && S) {
            TypeInterface_destroy(S->element_type, n->data);
        }
    }
    free(n->data);
    free(n);
}

static int _stack_node_set(const Stack S, _stack_node *n, const void *in)
{
    check_ptr(S);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        TypeInterface_destroy(S->element_type, n->data);
    } else {
        n->data = TypeInterface_allocate(S->element_type, 1);
        check(n->data != NULL, "Failed to allocate memory for new element.");
    }

    TypeInterface_copy(S->element_type, n->data, in);

    return 0;
error:
    return -1;
}

static inline int _stack_node_get(const Stack S, _stack_node *n, void *out)
{
    check_ptr(S);
    check_ptr(n);
    check_ptr(out);

    TypeInterface_copy(S->element_type, out, n->data);

    return 0;
error:
    return -1;
}

Stack Stack_new(TypeInterface *element_type)
{
    check_ptr(element_type);

    Stack S = malloc(sizeof(*S));
    check_alloc(S);

    S->top = NULL;
    S->size = 0;
    S->element_type = element_type;

    assert(!_stack_invariant(S));
    return S;
error:
    return NULL;
}

void Stack_delete(Stack S)
{
    if (S) {
        if (S->size) Stack_clear(S);
        free(S);
    }
}

void Stack_clear(Stack S)
{
    assert(!_stack_invariant(S));

    _stack_node *cur;
    _stack_node *next;

    for (cur = S->top; cur != NULL; cur = next) {
        next = cur->next;
        _stack_node_delete(S, cur);
    }
    S->top = NULL;
    S->size = 0;
}

int Stack_push(Stack S, const void *in)
{
    check_ptr(S);
    assert(!_stack_invariant(S));
    check_ptr(in);

    _stack_node *n = _stack_node_new();
    check(n != NULL, "Failed to make new node.");
    check(!_stack_node_set(S, n, in), "Failed to write data to new node.");

    n->next = S->top;
    S->top = n;
    ++S->size;

    assert(!_stack_invariant(S));
    return 0;
error:
    return -1;
}

int Stack_pop(Stack S, void *out)
{
    check_ptr(S);
    assert(!_stack_invariant(S));
    check_ptr(out);
    check(S->size > 0, "Attempt to pop from empty stack.");

    _stack_node *n = S->top;

    check(!_stack_node_get(S, n, out), "Failed to hand out value.");

    S->top = n->next;
    --S->size;

    _stack_node_delete(S, n);

    assert(!_stack_invariant(S));
    return 0;
error:
    return -1;
}
