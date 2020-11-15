#include "check.h"
#include "json.h"

String *List_to_json(const List *L, serialize_f serialize)
{
    String *temp = NULL;
    String *json = NULL;

    check_ptr(L);
    check_ptr(serialize);

    json = String_from_cstr("[");
    check(json != NULL, "Failed to create String for serialized data.");

    size_t count = 0;
    void *element;

    List_foreach(L, element) {
        temp = serialize(element);
        /* log_info("%s, json='%s', temp='%s'", __func__, json->data, temp->data); */
        check(temp != NULL,
                "Failed to serialize element at index %lu.", count);
        check(!String_append(json, temp),
                "Failed to append serialized element to result string.");
        /* log_info("%s, after appending json='%s'", __func__, json->data); */
        if (++count < List_size(L)) {
            check(!String_append_cstr(json, ", "), "Failed to append comma.");
        }
        String_delete(temp);
        temp = NULL;
    }
    check(!String_push_back(json, ']'), "Failed to push back trailing ].");

    return json;
error:
    if (temp) String_delete(temp);
    if (json) String_delete(json);
    return NULL;
}
