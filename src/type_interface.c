#include "check.h"
#include "str.h"
#include "type_interface.h"

void *t_allocate(const t_intf *T, size_t n)
{
    void *obj = malloc(n * T->size);
    check_alloc(obj);
    return obj;
error:
    return NULL;
}

void t_copy(const t_intf *T, void *dest, const void *src)
{
    if (T->copy) {
        T->copy(dest, src);
    } else {
        memmove(dest, src, T->size);
    }
}

void t_destroy(const t_intf *T, void *obj)
{
    if (T->destroy) T->destroy(obj);
}

int t_compare(const t_intf *T, const void *a, const void *b)
{
    return T->compare(a, b);
}

uint32_t t_hash(const t_intf *T, const void *obj)
{
    if (T->hash) {
        return T->hash(obj);
    } else {
        return jenkins_hash(obj, T->size);
    }
}

void t_print(const t_intf *T, FILE *stream, const void *obj)
{
    if (T->print) T->print(stream, obj);
}

/* Predefined type interfaces: */

t_intf String_type = {
    .size = sizeof(String),
    .copy = String_copy_to,
    .destroy = String_destroy,
    .compare = String_compare,
    .hash = String_hash,
    .print = String_printf
};

/* Built-in types */

int int_compare(const void *a, const void *b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

uint32_t int_hash(const void *i)
{
    return jenkins_hash(i, sizeof(int));
}

void int_print(FILE *stream, const void *i)
{
    fprintf(stream, "%d", *(int*)i);
}

t_intf int_type = {
    .size = sizeof(int),
    .copy = NULL,
    .destroy = NULL,
    .compare = int_compare,
    .hash = int_hash,
    .print = int_print
};

int pointer_compare(const void *a, const void *b)
{
    return a < b ? -1 : a > b ? 1 : 0;
}

t_intf pointer_type = {
    .size = sizeof(void*),
    .copy = NULL,
    .destroy = NULL,
    .compare = pointer_compare,
    .hash = NULL,
    .print = NULL
};
