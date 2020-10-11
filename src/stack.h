#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "container_tools.h"

struct __StackNode;
typedef struct __StackNode {
    struct __StackNode* next;
    char* data;
} __StackNode;

typedef struct Stack {
    __StackNode* top;
    __destroy_f destroy;
    size_t element_size;
    size_t size;
} Stack;

#define Stack_top(L) (L)->top->data
#define Stack_size(L) (L)->size

int Stack_init(Stack* s, const size_t element_size, __destroy_f destroy);
void Stack_clear(Stack* s);

#define Stack_push_front(l, in) Stack_insert((l), 0, (in))
int Stack_push(Stack* s, const void* in);
int Stack_pop(Stack* s, void* out);

__StackNode* __N;
#define Stack_foreach(L, D) \
    for (__N = (L)->first, D = (void*)(__N->data); \
            __N != NULL; __N = __N->next, D = __N ? (void*)(__N->data) : NULL)

#endif // _stack_h
