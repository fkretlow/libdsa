#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "container_tools.h"

struct _stack_node;
typedef struct _stack_node {
    struct _stack_node *next;
    char *data;
} _stack_node;

typedef struct _stack {
    _stack_node *top;
    size_t element_size;
    size_t size;
    copy_f copy_element;
    destroy_f destroy_element;
} _stack;

typedef _stack *Stack;

#define Stack_top(S) ((S)->top ? (void*)(S)->top->data : NULL)
#define Stack_size(S) (S)->size
#define Stack_empty(S) ((S)->size == 0)

Stack Stack_new(const size_t element_size,
                copy_f copy_element,
                destroy_f destroy_element);
void Stack_delete(Stack S);
void Stack_clear(Stack S);

int Stack_push(Stack S, const void *in);
int Stack_pop(Stack S, void *out);

#endif // _stack_h
