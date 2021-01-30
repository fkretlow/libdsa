/*************************************************************************************************
 *
 * stack.h
 *
 * LIFO queue implemented in terms of a singly-linked list. Simple adapter for forward_list.
 *
 * Author: Florian Kretlow, 2020
 * Licensed under the MIT License.
 *
 ************************************************************************************************/

#ifndef _stack_h
#define _stack_h

#include "forward_list.h"

typedef flist stack;

#define stack_top(S)            flist_front(S)
#define stack_count(S)          (S)->count
#define stack_empty(S)          ((S)->count == 0)

#define stack_initialize(S, dt) flist_initialize(S, dt)
#define stack_new(dt)           flist_new(dt)
#define stack_delete(S)         flist_delete(S)
#define stack_destroy(S)        flist_destroy(S)
#define stack_clear(S)          flist_clear(S)

#define stack_push(S, v)        flist_push_front(S, v)
#define stack_pop(S, out)       flist_pop_front(S, out)

#endif // _stack_h
