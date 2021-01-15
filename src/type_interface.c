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

void t_move(const t_intf *T, void *dest, void *src)
{
    if (T->move) {
        T->move(dest, src);
    } else {
        memmove(dest, src, T->size);
        memset(src, 0, T->size);
    }
}

int t_swap(const t_intf *T, void *a, void *b)
{
    if (T->swap) {
        T->swap(a, b);
    } else {
        void *temp = malloc(T->size);
        check_alloc(temp);
        t_move(T, temp, a);
        t_move(T, a, b);
        t_move(T, b, temp);
        free(temp);
    }
    return 0;
error:
    return -1;
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

t_intf str_type = {
    .size = sizeof(str),
    .copy = str_copy_to,
    .destroy = str_destroy,
    .compare = str_compare,
    .hash = str_hash,
    .print = str_print
};

/* Built-in types */

int int_compare(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
    /* return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0; */
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
