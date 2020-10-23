#ifndef _container_tools_h
#define _container_tools_h

typedef void (*__destroy_f)(void *element);
typedef int (*__traverse_f)(const void *element);
#define __max(A, B) (A > B ? A : B)

#endif // _container_tools_h
