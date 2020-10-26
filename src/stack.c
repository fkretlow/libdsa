#include "stack.h"
#include <assert.h>

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
        if (n->data && S && S->destroy_element) {
            S->destroy_element(*(void**)n->data);
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
        if (S->destroy_element) {
            S->destroy_element(*(void**)n->data);
        }
    } else {
        n->data = malloc(S->element_size);
        check_alloc(n->data);
    }

    if (S->copy_element) {
        S->copy_element(n->data, in);
    } else {
        memmove(n->data, in, S->element_size);
    }

    return 0;
error:
    return -1;
}

static inline int _stack_node_get(const Stack S, _stack_node *n, void *out)
{
    check_ptr(S);
    check_ptr(n);
    check_ptr(out);

    if (S->copy_element) {
        S->copy_element(out, n->data);
    } else {
        memmove(out, n->data, S->element_size);
    }

    return 0;
error:
    return -1;
}

Stack Stack_new(const size_t element_size,
                copy_f copy_element,
                destroy_f destroy_element)
{
    _stack *S = malloc(sizeof(*S));
    check_alloc(S);

    S->top = NULL;
    S->element_size = element_size;
    S->size = 0;
    S->copy_element = copy_element;
    S->destroy_element = destroy_element;

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
