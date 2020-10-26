#include "json.h"

String List_to_json(const List *l, serialize_f serialize)
{
    String temp = NULL;
    String json = NULL;

    check_ptr(l);
    check_ptr(serialize);

    json = String_from_cstr("[");
    check(json != NULL, "Failed to create String for serialized data.");

    size_t count = 0;
    void *element;

    List_foreach(l, element) {
        String temp = serialize(element);
        /* debug("%s, json='%s', temp='%s'", __func__, json->data, temp->data); */
        check(temp != NULL,
                "Failed to serialize element at index %lu.", count);
        check(!String_append(json, temp),
                "Failed to append serialized element to result string.");
        /* debug("%s, after appending json='%s'", __func__, json->data); */
        if (++count < List_size(l)) {
            check(!String_append_cstr(json, ", "), "Failed to append comma.");
        }
        String_delete(temp);
    }
    check(!String_append_cstr(json, "]"), "Failed to append trailing ].");

    return json;
error:
    if (temp) String_delete(temp);
    if (json) String_delete(json);
    return NULL;
}
