#ifndef _json_h
#define _json_h

#include "list.h"
#include "str.h"

typedef String *(*serialize_f)(const void *element);

// The address of a new string is stored in json_out.
String *List_to_json(const List *L, serialize_f serialize);

#endif // _json_h
