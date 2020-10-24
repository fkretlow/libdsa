#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "container_tools.h"

struct StackNode;
typedef struct StackNode {
    struct StackNode *next;
    char *data;
} StackNode;

typedef struct Stack {
    StackNode *top;
    _destroy_f destroy;
    size_t element_size;
    size_t size;
} Stack;

#define Stack_top(S) ((S)->top ? (S)->top->data : NULL)
#define Stack_size(S) (S)->size
#define Stack_empty(S) ((S)->size == 0)

int Stack_init(Stack *s, const size_t element_size, _destroy_f destroy);
void Stack_clear(Stack *s);

int Stack_push(Stack *s, const void *in);
int Stack_pop(Stack *s, void *out);

#endif // _stack_h
