#ifndef _sort_tools_h
#define _sort_tools_h

typedef int (*_compare_f)(const void *a, const void *b);

void _swap(char *a, char *b, size_t size, char *temp);

void _insertionsort(char *base,
                     size_t start, size_t end,
                     size_t size,
                     _compare_f compare,
                     char *temp);

#endif // _sort_tools_h
