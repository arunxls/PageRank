#include "stdafx.h"
#include "GraphReader.h"

GraphReader::GraphReader(char* buffer_start, char* buffer_end, char * file_name)
{
    this->total_read = 0;
    this->total_write = 0;

    this->out_degree = 0;
    this->node_count = 0;

    this->FR = new FileReader(file_name);

    this->buffer_start = buffer_start;
    this->buffer_end = buffer_end;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
}

GraphReader::GraphReader()
{
    this->total_read = 0;
    this->total_write = 0;
}

GraphReader::~GraphReader()
{
    delete this->FR;
}

bool GraphReader::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

uint32 GraphReader::nextNode()
{
    if (this->start == this->end) {
        this->load();
    }

    uint32 node = *(uint32*) this->start;
    this->start += sizeof(HeaderGraph);

    return node;
}

void GraphReader::load()
{
    uint32 bytesTransferred = 0;

    this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);

    this->end = this->buffer_start + bytesTransferred;
    this->total_read += bytesTransferred;
}