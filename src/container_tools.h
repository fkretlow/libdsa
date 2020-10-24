#ifndef _container_tools_h
#define _container_tools_h

#include "str.h"

typedef void (*_destroy_f)(void *element);
typedef String (*_serialize_f)(const void *element);
typedef int (*_traverse_f)(const void *element);
#define _max(A, B) (A > B ? A : B)

#endif // _container_tools_h
