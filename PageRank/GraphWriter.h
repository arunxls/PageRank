#pragma once

#include "include.h"
#include "FileWriter.h"

class GraphWriter
{
public:
    char* buffer_start;
    char* buffer_end;
    uint32* start;
    uint32* end;

    uint32* truncatedPointer;
    uint32 truncatedCounter;

    uint64 total_read;
    uint64 total_write;

    FileWriter* FW;

    GraphWriter();
    GraphWriter(char* file_name);
    ~GraphWriter();

    void writeToDisk();
    void writeHeader(uint32 hash, uint32 len);
    void writeTruncated(uint32 node);
};

