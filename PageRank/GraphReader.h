#pragma once

#include "include.h"
#include "FileReader.h"

#define GRAPH_READ_BUFFER 16

class GraphReader
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint64 total_read;
    uint64 total_write;

    bool createNodeHash;
    bool async;
    FileReader* FR;

    GraphReader(char * file_name);
    GraphReader(char * file_name, int bufferSize);
    GraphReader();
    ~GraphReader();

    uint32 size() { return GRAPH_READ_BUFFER*_1_MB; }
    bool has_next();
    HeaderGraph currentHeader();
    uint32 currentNeighbour();
    void nextHeader();
    void nextNeighbour();
    uint32 remainingBuffer();
    void copyRange(char*& dst);
    void load();
private:
    char* alloc_start;
};

