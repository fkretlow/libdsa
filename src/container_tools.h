#ifndef _container_tools_h
#define _container_tools_h

typedef void (*__destroy_f)(void* element);
typedef unsigned long (*__hash_f)(const void* data, size_t size);

#endif // _container_tools_h
