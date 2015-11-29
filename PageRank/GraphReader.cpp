#include "stdafx.h"
#include "GraphReader.h"

GraphReader::GraphReader()
{
    this->total_read = 0;
    this->total_write = 0;

    this->buffer_start = new char[GRAPH_READER_SIZE*_1_MB];
    this->buffer_end = this->buffer_start + GRAPH_READER_SIZE*_1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

GraphReader::~GraphReader()
{
    delete this->FR;
    delete[] this->buffer_start;
}

bool GraphReader::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

uint32 GraphReader::next_node(const HANDLE* gGraph_EMPTY, CONDITION_VARIABLE* GRAPH_LOADED, CRITICAL_SECTION* GRAPH_LOCK, GraphReader* graph)
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

uint32 GraphReader::next_node()
{
    if (this->start == this->end)
    {
        this->load();
    }

    uint32 currentHash = *(uint32*)this->start;
    this->start += sizeof(uint32);

    return currentHash;
}

void GraphReader::load()
{
    uint32 bytesTransferred = 0;

    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);

    this->end = this->buffer_start + bytesTransferred;
    this->start = this->buffer_start;
    this->total_read += bytesTransferred;
}

void GraphReader::init(char * file_name, bool buffer)
{
    this->FR = new FileReader(file_name);
}
