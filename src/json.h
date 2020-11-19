#ifndef _json_h
#define _json_h

#include "list.h"
#include "str.h"

typedef str *(*serialize_f)(const void *element);

// The address of a new string is stored in json_out.
str *list_to_json(const list *L, serialize_f serialize);

#endif // _json_h
