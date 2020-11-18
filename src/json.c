#include "check.h"
#include "json.h"

str *List_to_json(const List *L, serialize_f serialize)
{
    str *temp = NULL;
    str *json = NULL;

    check_ptr(L);
    check_ptr(serialize);

    json = str_from_cstr("[");
    check(json != NULL, "Failed to create str for serialized data.");

    size_t count = 0;
    void *element;

    List_foreach(L, element) {
        temp = serialize(element);
        /* log_info("%s, json='%s', temp='%s'", __func__, json->data, temp->data); */
        check(temp != NULL,
                "Failed to serialize element at index %lu.", count);
        check(!str_append(json, temp),
                "Failed to append serialized element to result string.");
        /* log_info("%s, after appending json='%s'", __func__, json->data); */
        if (++count < List_size(L)) {
            check(!str_append_cstr(json, ", "), "Failed to append comma.");
        }
        str_delete(temp);
        temp = NULL;
    }
    check(!str_push_back(json, ']'), "Failed to push back trailing ].");

    return json;
error:
    if (temp) str_delete(temp);
    if (json) str_delete(json);
    return NULL;
}
