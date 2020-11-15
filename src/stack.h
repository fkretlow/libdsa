#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "type_interface.h"

struct StackNode;
typedef struct StackNode {
    struct StackNode *next;
    char *data;
} StackNode;

typedef struct Stack {
    StackNode *top;
    size_t size;
    t_intf *element_type;
} Stack;

#define Stack_top(S) ((S)->top ? (void*)(S)->top->data : NULL)
#define Stack_size(S) (S)->size
#define Stack_empty(S) ((S)->size == 0)

Stack *Stack_new(t_intf *element_type);
int Stack_initialize(Stack *S, t_intf *element_type);
void Stack_delete(Stack *S);
void Stack_clear(Stack *S);

int Stack_push(Stack *S, const void *in);
int Stack_pop(Stack *S, void *out);

#endif // _stack_h
