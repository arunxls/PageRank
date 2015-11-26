#pragma once

#include "include.h"
#include "FileReader.h"

class GraphReader
{
public:
    char* buffer_start;
    char* buffer_end;
    char* start;
    char* end;

    uint32 out_degree;

    uint64 total_read;
    uint64 total_write;

    FileReader* FR;

    GraphReader(char * file_name, bool buffered);
    GraphReader();
    ~GraphReader();

    bool has_next();
    uint32 nextNode();
    void load();
private:
    uint32 node_count;
};