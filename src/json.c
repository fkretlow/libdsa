#include "json.h"

int List_to_json(const List *l, _serialize_f serialize, String **json_out)
{
    String *element = NULL;

    check_ptr(l);
    check_ptr(serialize);
    check_ptr(json_out);

    check(!String_new(json_out), "String_new failed.");
    check(!String_append_cstr(json_out, "["), "String_append_cstr failed.");
    check(!String_new(&element), "String_new failed.");

    size_t count = 0;
    void *data;

    List_foreach(l, data) {
        ++count;
        check(!serialize(data, element), "serialization failed");
        check(!String_append(json_out, element), "String_append failed.");
        if (count < List_size(l)) {
            check(!String_append_cstr(json_out, ", "), "String_append_cstr failed");
        }
    }
    check(!String_append_cstr(json_out, "]"), "String_append_cstr failed.");

    String_delete(element);
    return 0;
error:
    if (element) String_delete(element);
    return -1;
}
