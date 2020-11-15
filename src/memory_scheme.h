#ifndef _memory_scheme_h
#define _memory_scheme_h

#include "type_interface.h"

union VariableSizeData {
    struct {
        char data[sizeof(size_t) + sizeof(char*)];
    } internal;
    struct {
        char *data;
        size_t capacity;
    } external;
};

typedef struct mscheme {
    t_intf *key_type;
    t_intf *value_type;
    unsigned char key_external   : 1;
    unsigned char value_external : 1;
    unsigned char value_offset   : 6; /* offset <= 16 => 6 bits suffice */
} mscheme;

int     generate_memory_scheme(mscheme *s, t_intf *kt, t_intf *vt);

int     set_key         (char *data, const mscheme *s, const void *k);
void *  get_key         (char *data, const mscheme *s);
void    destroy_key     (char *data, const mscheme *s);

int     set_value       (char *data, const mscheme *s, const void *v);
void *  get_value       (char *data, const mscheme *s);
void    destroy_value   (char *data, const mscheme *s);

#endif // _memory_scheme_h
