#pragma once

#include <windows.h>

#define _1_MB 1000000
#define _1_GB 1000000000
#define DEBUG 0

#define BUFFERSIZE 496
#define TOTAL_NODES 86533762
#define TOTAL_NODES_WITH_OUT_DEGREE 25943372
#define GRAPH_READER_SIZE 32

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned short   ushort;

#pragma pack(push,1) // change struct packing to 1 byte
class HeaderGraph {
public:
    uint32 hash;
    uint32 len;

    uint32 size()
    {
        return sizeof(HeaderGraph) + (sizeof(uint32)*this->len);
    }
};

class TruncatedHeaderGraph {
public:
    uint32 hash;
    uint32 original_len;
    uint32 current_len;

    uint32 size()
    {
        return sizeof(TruncatedHeaderGraph) + (sizeof(uint32)*this->current_len);
    }
};
#pragma pack(pop)
