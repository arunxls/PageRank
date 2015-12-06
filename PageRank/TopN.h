#pragma once
#include <algorithm>
#include "include.h"
#include "FileWriter.h"
#include <stdio.h>

#define OUTPUT_RANK 1

class TopN
{
public:
    char* start;
    char* end;
    char* buffer_start;
    char* buffer_end;

    uint64 total_write;
    uint64 total_read;

    inline TopN()
    {
        this->buffer_start = new char[OUTPUT_RANK * _1_MB];
        this->buffer_end = this->buffer_start + OUTPUT_RANK * _1_MB;

        this->start = this->buffer_start;
        this->end = this->start;

        this->total_read = 0;
        this->total_write = 0;
    }

    inline ~TopN()
    {
        delete[] this->buffer_start;
    }

    inline void write()
    {
        FileWriter FW("output.txt");
        FW.write(this->buffer_start, this->start - this->buffer_start);
        this->total_write += this->start - this->buffer_start;
    }
    
    inline void put(uint32 index, uint32 hash, float value)
    {
        this->start += sprintf(this->start, "%3d  %8d %f\r\n", index, hash, value);
    }
};

