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

typedef union MappingData {
    struct {
        char data[2 * sizeof(char*)];
    } internal;
    struct {
        char *key;
        char *value;
    } external;
} MappingData;

int MappingData_set_key(MappingData *data, const int external,
                        const TypeInterface *key_type,
                        const void *key);
void MappingData_get_key(const MappingData *data, const int external,
                         const TypeInterface *key_type,
                         void *key_out);
void MappingData_destroy_key(MappingData *data, const int external,
                             const TypeInterface *key_type);
void *MappingData_key_address(MappingData *data, const int external);

int MappingData_set_value(MappingData *data, const int external,
                          const TypeInterface *key_type,
                          const TypeInterface *value_type,
                          const void *value);
void MappingData_get_value(const MappingData *data, const int external,
                           const TypeInterface *key_type,
                           const TypeInterface *value_type,
                           void *value_out);
void MappingData_destroy_value(MappingData *data, const int external,
                               const TypeInterface *key_type,
                               const TypeInterface *value_type);
void *MappingData_value_address(MappingData *data, const int external,
                                const TypeInterface *key_type);
#endif // _node_data_ha
