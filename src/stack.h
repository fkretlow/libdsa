#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "container_tools.h"

struct __StackNode;
typedef struct __StackNode {
    struct __StackNode *next;
    char *data;
} __StackNode;

typedef struct Stack {
    __StackNode *top;
    __destroy_f destroy;
    size_t element_size;
    size_t size;
} Stack;

#define Stack_top(S) ((S)->top ? (S)->top->data : NULL)
#define Stack_size(S) (S)->size
#define Stack_empty(S) ((S)->size == 0)

int Stack_init(Stack *s, const size_t element_size, __destroy_f destroy);
void Stack_clear(Stack *s);

int Stack_push(Stack *s, const void *in);
int Stack_pop(Stack *s, void *out);

#endif // _stack_h
