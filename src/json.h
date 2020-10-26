#ifndef _json_h
#define _json_h

#include "container_tools.h"
#include "list.h"
#include "str.h"

// The address of a new string is stored in json_out.
String List_to_json(const List *l, serialize_f serialize);

#endif // _json_h
