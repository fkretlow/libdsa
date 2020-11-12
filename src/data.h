#ifndef _data_h
#define _data_h

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

#endif // _data_h
