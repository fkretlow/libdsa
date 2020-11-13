#ifndef _node_data_h
#define _node_data_h

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

union MappingData {
    struct {
        char data[2 * sizeof(char*)];
    } internal;
    struct {
        char *key;
        char *value;
    } external;
};

int MappingData_set_key(MappingData *d, int external,
                        TypeInterface *key_type,
                        const void *k);

int MappingData_get_key(MappingData *d, int external,
                        TypeInterface *key_type,
                        void *k_out);

int MappingData_destroy_key(MappingData *d,
                            TypeInterface *key_type, int external);

void *MappingData_key_address(MappingData *d, int external);

int MappingData_set_value(MappingData *d, int external,
                          TypeInterface *key_type, TypeInterface *value_type,
                          const void *v);

int MappingData_get_value(MappingData *d, int external,
                          TypeInterface *key_type, TypeInterface *value_type,
                          void *k_out);

int MappingData_destroy_value(MappingData *d, int external,
                              TypeInterface *key_type, TypeInterface *value_type);

void *MappingData_value_address(MappingData *d, int external, TypeInterface *key_type);

#endif // _node_data_h
