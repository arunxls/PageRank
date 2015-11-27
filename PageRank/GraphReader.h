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

    uint64 total_read;
    uint64 total_write;

    FileReader* FR;

    GraphReader();
    ~GraphReader();

    bool has_next();
    uint32 next_node(const HANDLE*, CONDITION_VARIABLE*, CRITICAL_SECTION*, GraphReader* graph);
    void load();
    void init(char* file_name, bool buffer);
};