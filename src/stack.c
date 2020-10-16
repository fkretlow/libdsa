#include "stack.h"
#include <assert.h>

static inline int __Stack_invariant(const Stack *s)
{
    if (s->top) check(s->size > 0, "Stack invariant violated: s->top && s->size == 0");
    return 0;
error:
    return -1;
}

static int __StackNode_new(__StackNode **node_out)
{
    __StackNode *new = calloc(1, sizeof(*new));
    check_alloc(new);
    *node_out = new;
    return 0;
error:
    return -1;
}

static void __StackNode_delete(const Stack *s, __StackNode *n)
{
    if (n) {
        if (n->data && s && s->destroy) {
            s->destroy(n->data);
        }
    }
    free(n->data);
    free(n);
}

static int __StackNode_set(const Stack *s, __StackNode *n, const void *in)
{
    check_ptr(s);
    check_ptr(n);
    check_ptr(in);

    if (n->data) {
        if (s->destroy) {
            s->destroy(n->data);
        }
    } else {
        n->data = malloc(s->element_size);
        check_alloc(n->data);
    }
    memmove(n->data, in, s->element_size);

    return 0;
error:
    return -1;
}

int Stack_init(Stack *s, const size_t element_size, __destroy_f destroy)
{
    check_ptr(s);

    s->top = NULL;
    s->element_size = element_size;
    s->size = 0;
    s->destroy = destroy;

    assert(!__Stack_invariant(s));
    return 0;
error:
    return -1;
}

void Stack_clear(Stack *s)
{
    assert(!__Stack_invariant(s));

    __StackNode *cur;
    __StackNode *next;

    for (cur = s->top; cur != NULL; cur = next) {
        next = cur->next;
        __StackNode_delete(s, cur);
    }
    s->size = 0;
}

int Stack_push(Stack *s, const void *in)
{
    check_ptr(s);
    assert(!__Stack_invariant(s));
    check_ptr(in);

    __StackNode *n;
    check(!__StackNode_new(&n), "Failed to make new node.");
    check(!__StackNode_set(s, n, in), "Failed to write data to new node.");

    n->next = s->top;
    s->top = n;
    ++s->size;

    assert(!__Stack_invariant(s));
    return 0;
error:
    return -1;
}

int Stack_pop(Stack *s, void *out)
{
    check_ptr(s);
    assert(!__Stack_invariant(s));
    check_ptr(out);
    check(s->size > 0, "Attempt to pop from empty stack.");

    __StackNode *n = s->top;

    memmove(out, n->data, s->element_size);

    s->top = n->next;
    --s->size;

    __StackNode_delete(s, n);

    assert(!__Stack_invariant(s));
    return 0;
error:
    return -1;
}
