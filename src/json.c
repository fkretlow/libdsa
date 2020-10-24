#include "json.h"

int List_to_json(const List *l, _serialize_f serialize, String **json_out)
{
    String *temp = NULL;

    check_ptr(l);
    check_ptr(serialize);
    check_ptr(json_out);

    check(!String_new(json_out), "String_new failed.");
    check(!String_set(*json_out, "[", 1), "String_set failed.");
    check(!String_new(&temp), "String_new failed.");

    size_t count = 0;
    void *element;

    List_foreach(l, element) {
        ++count;
        serialize(element, temp);
        check(!String_append(*json_out, temp), "String_append failed.");
        if (count < List_size(l)) {
            check(!String_append_cstr(*json_out, ", "), "String_append_cstr failed");
        }
    }
    check(!String_append_cstr(*json_out, "]"), "String_append_cstr failed.");

    String_delete(temp);
    return 0;
error:
    if (temp) String_delete(temp);
    return -1;
}
