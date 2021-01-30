/*************************************************************************************************
 *
 * stack.h
 * Interface of a stack (LIFO queue).
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _stack_h
#define _stack_h

#include <stdlib.h>
#include <string.h>

#include "type_interface.h"

struct stackn;
typedef struct stackn {
    struct stackn *next;
    char *data;
} stackn;

typedef struct stack {
    stackn *top;
    size_t count;
    t_intf *data_type;
} stack;

#define stackn_size(S) (sizeof(stackn) + t_size((S)->data_type))

#define stack_top(S) ((S)->top ? (void*)(S)->top->data : NULL)
#define stack_count(S) (S)->count
#define stack_empty(S) ((S)->count == 0)

int         stack_initialize    (stack *S, t_intf *dt);
stack *     stack_new           (t_intf *dt);
void        stack_delete        (stack *S);
void        stack_clear         (stack *S);

int         stack_push          (stack *S, const void *in);
int         stack_pop           (stack *S, void *out);

#endif // _stack_h
