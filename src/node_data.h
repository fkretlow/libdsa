#ifndef _node_data_h
#define _node_data_h

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

union FixedSizeData {
    struct {
        char data[sizeof(char*)];
    } internal;
    struct {
        char *data;
    } external;
};

#define MAPPING_DATA_SIZE (2 * sizeof(void*))
#define MAPPING_DATA_KEY_EXTERNAL   1
#define MAPPING_DATA_VALUE_EXTERNAL 2

typedef struct {
    TypeInterface *key_type;
    TypeInterface *value_type;
    unsigned char key_external   : 1;
    unsigned char value_external : 1;
    unsigned char value_offset   : 6; /* offset <= 16, so 6 bits are enough. */
} MemoryScheme;

int MemoryScheme_initialize(MemoryScheme *scheme,
                            TypeInterface *key_type, TypeInterface *value_type);

int MappingData_set_key(char *data, const MemoryScheme *scheme, const void *key);
void MappingData_get_key(const char *data, const MemoryScheme *scheme, void *key_out);
void MappingData_destroy_key(char *data, const MemoryScheme *scheme);
void *MappingData_key_address(char *data, const MemoryScheme *scheme);

int MappingData_set_value(char *data, const MemoryScheme *scheme, const void *value);
void MappingData_get_value(const char *data, const MemoryScheme *scheme, void *value_out);
void MappingData_destroy_value(char *data, const MemoryScheme *scheme);
void *MappingData_value_address(char *data, const MemoryScheme *scheme);
#endif // _node_data_h
