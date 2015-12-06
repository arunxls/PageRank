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

    inline GraphReader()
    {
        this->total_read = 0;
        this->total_write = 0;

        this->buffer_start = new char[GRAPH_READER_SIZE*_1_MB];
        this->buffer_end = this->buffer_start + GRAPH_READER_SIZE*_1_MB;

        this->start = this->buffer_start;
        this->end = this->buffer_start;
        this->FR = NULL;
    }
    
    inline ~GraphReader()
    {
        delete this->FR;
        delete[] this->buffer_start;
    }

    inline bool has_next()
    {
        return this->FR->has_next() ? true : this->start < this->end;
    }
    
    inline uint32 next_node(const HANDLE* gGraph_EMPTY, CONDITION_VARIABLE* GRAPH_LOADED, CRITICAL_SECTION* GRAPH_LOCK, GraphReader* graph)
    {
        if (this->start == this->end)
        {
            ReleaseSemaphore(*gGraph_EMPTY, 1, NULL);
            SleepConditionVariableCS(GRAPH_LOADED, GRAPH_LOCK, INFINITE);
            this->start = graph->start;
            this->end = graph->end;
        }

        uint32 currentHash = *(uint32*)this->start;
        this->start += sizeof(uint32);

        return currentHash;
    }

    inline uint32 next_node() {
        if (this->start == this->end)
        {
            this->load();
        }

        uint32 currentHash = *(uint32*)this->start;
        this->start += sizeof(uint32);

        return currentHash;
    }
    
    inline void load()
    {
        uint32 bytesTransferred = 0;

        this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);

        this->end = this->buffer_start + bytesTransferred;
        this->start = this->buffer_start;
        this->total_read += bytesTransferred;
    }

    inline void init(char* file_name, bool buffer)
    {
        if (this->FR != NULL) 
        {
            delete this->FR;
        }
        this->FR = new FileReader(file_name);
    }

    inline void reset() 
    { 
        this->FR->offset_overall = 0; 
    }

    inline void resize(uint32 size)
    {
        delete[] this->buffer_start;
        this->buffer_start = new char[size*_1_MB];
        this->buffer_end = this->buffer_start + size*_1_MB;

        this->start = this->buffer_start;
        this->end = this->buffer_start;
    }
};