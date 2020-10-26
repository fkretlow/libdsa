#ifndef _container_tools_h
#define _container_tools_h

#include "str.h"

typedef void (*destroy_f)(void *element);
typedef void (*copy_f)(void *dest, const void *src);
typedef String (*serialize_f)(const void *element);
typedef int (*traverse_f)(const void *element);
#define _max(A, B) (A > B ? A : B)

#endif // _container_tools_h
