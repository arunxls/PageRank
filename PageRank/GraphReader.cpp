#include "stdafx.h"
#include "GraphReader.h"

GraphReader::GraphReader(char * file_name, bool buffered)
{
    this->total_read = 0;
    this->total_write = 0;

    this->out_degree = 0;
    this->node_count = 0;

    this->FR = new FileReader(file_name);

    this->buffer_start = new char[GRAPH_READER_SIZE*_1_MB];
    this->buffer_end = this->buffer_start + GRAPH_READER_SIZE*_1_MB;

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