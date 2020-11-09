#include "type_interface.h"

void *TypeInterface_allocate(TypeInterface *T, size_t n)
{
    void *obj = malloc(n * T->size);
    check_alloc(obj);
    return obj;
error:
    return NULL;
}

void TypeInterface_copy(TypeInterface *T, void *dest, const void *src)
{
    if (T->copy) {
        T->copy(dest, src);
    } else {
        memmove(dest, src, T->size);
    }
}

void TypeInterface_destroy(TypeInterface *T, void *obj)
{
    if (T->destroy) T->destroy(obj);
}

int TypeInterface_compare(TypeInterface *T, const void *a, const void *b)
{
    return T->compare(a, b);
}

unsigned long TypeInterface_hash(TypeInterface *T, const void *obj)
{
    if (T->hash) {
        return T->hash(obj);
    } else {
        return jenkins_hash(obj, T->size);
    }
}

void TypeInterface_print(TypeInterface *T, FILE *stream, const void *obj)
{
    if (T->print) T->print(stream, obj);
}

/* Predefined type interfaces: */

/* String */

void _string_ptr_copy(void *dest, const void *src)
{
    *(String *)dest = String_copy(*(String *)src);
}

void _string_ptr_delete(void *s) { String_delete(*(String *)s); }

int _string_ptr_compare(const void *s1, const void *s2)
{
    return String_compare(*(String *)s1, *(String *)s2);
}

unsigned long _string_ptr_hash(const void *s)
{
    return jenkins_hash((*(String *)s)->data, (*(String *)s)->size);
}

void _string_ptr_print(FILE *stream, const void *s)
{
    fputc('"', stream);
    fprintf(stream, "%s", (*(String *)s)->data);
    fputc('"', stream);
}

TypeInterface String_type = {
    .size = sizeof(String),
    .copy = _string_ptr_copy,
    .destroy = _string_ptr_delete,
    .compare = _string_ptr_compare,
    .hash = _string_ptr_hash,
    .print = _string_ptr_print
};

/* Built-in types */

int int_compare(const void *a, const void *b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

unsigned long int_hash(const void *i)
{
    return jenkins_hash(i, sizeof(int));
}

void int_print(FILE *stream, const void *i)
{
    fprintf(stream, "%d", *(int*)i);
}

TypeInterface int_type = {
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

TypeInterface pointer_type = {
    .size = sizeof(void*),
    .copy = NULL,
    .destroy = NULL,
    .compare = pointer_compare,
    .hash = NULL,
    .print = NULL
};
