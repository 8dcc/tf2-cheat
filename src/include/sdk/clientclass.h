#ifndef CLIENTCLASS_H_
#define CLIENTCLASS_H_ 1

#include <stdint.h>

/*
 * See:
 * https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/dt_recv.h#L87
 */

typedef enum ESendPropType {
    INT = 0,
    FLOAT,
    VECTOR,
    VECTOR2D,
    STRING,
    ARRAY,
    DATATABLE,
    INT64,
    SENDPROPTYPEMAX
} ESendPropType;

typedef struct RecvTable_s {
    struct RecvProp_s* props;
    int propsCount;
    void* decoder;
    char* tableName;
    bool initialized;
    bool inMainList;
} RecvTable;

typedef struct RecvProp_s {
    char* varName;
    ESendPropType recvType;
    int flags;
    int stringBufferSize;
    bool insideArray;
    const void* extraData;
    struct RecvProp_s* arrayProp;
    void* arrayLengthProxyFn;
    void* proxyFn;
    void* dataTableProxyFn;
    RecvTable* dataTable;
    int offset;
    int elementStride;
    int elements;
    const char* parentArrayPropName;
} RecvProp;

typedef struct ClientClass_s {
    void* CreateClientClassFn;
    void* CreateEventFn;
    const char* network_name;
    RecvTable* recv_table;
    struct ClientClass_s* next;
    int class_id;
    const char* map_class_name;
} ClientClass;

#endif /* CLIENTCLASS_H_ */
