#ifndef _json_h
#define _json_h

#include "container_tools.h"
#include "list.h"
#include "str.h"

int List_to_json(const List *l, _serialize_f serialize, String **json_out);

#endif // _json_h
