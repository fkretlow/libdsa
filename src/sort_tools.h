#ifndef _sort_tools_h
#define _sort_tools_h

typedef int (*__compare_f)(const void *a, const void *b);

void __swap(char *a, char *b, size_t size, char *temp);

void __insertionsort(char *base,
                     size_t start, size_t end,
                     size_t size,
                     __compare_f compare,
                     char *temp);

#endif // _sort_tools_h
