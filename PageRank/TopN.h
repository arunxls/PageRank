#pragma once
#include <algorithm>
#include "include.h"

#define OUTPUT_RANK 50

class TopN
{
public:
    char* start;
    char* end;
    char* buffer_start;
    char* buffer_end;

    uint64 total_write;
    uint64 total_read;

    TopN();
    ~TopN();

    void write();
    void put(uint32 index, uint32 hash, float value);
};

