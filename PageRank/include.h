#pragma once

#define _1_MB 1000000
#define _1_GB 1000000000
#define DEBUG 1
#define _HAS_ITERATOR_DEBUGGING 1

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef __int32 int32;
typedef unsigned short   ushort;

#pragma pack(push,1) // change struct packing to 1 byte
class HeaderGraph {
public:
    uint32 hash;
    uint32 len;

    bool operator==(const HeaderGraph& a) const
    {
        return hash == a.hash;
    }

    uint32 size()
    {
        return sizeof(HeaderGraph) + (sizeof(uint32)*this->len);
    }
};
#pragma pack(pop)
