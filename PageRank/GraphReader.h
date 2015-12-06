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
    inline uint32 next_node() {
        if (this->start == this->end)
        {
            this->load();
        }

        uint32 currentHash = *(uint32*)this->start;
        this->start += sizeof(uint32);

        return currentHash;
    }
    void load();
    void init(char* file_name, bool buffer);
    void reset() { this->FR->offset_current_read = 0; this->FR->offset_overall = 0; }
};