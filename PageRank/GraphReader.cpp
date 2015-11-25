#include "stdafx.h"
#include "GraphReader.h"

GraphReader::GraphReader(char * file_name)
{
    this->total_read = 0;
    this->total_write = 0;

    this->createNodeHash = createNodeHash;
    this->FR = new FileReader(file_name);

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(HeaderGraph);

    this->alloc_start = new char[GRAPH_READ_BUFFER*_1_MB + extra_buffer];

    this->buffer_start = this->alloc_start + extra_buffer;
    this->buffer_end = this->buffer_start + GRAPH_READ_BUFFER*_1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
    this->async = false;
}

GraphReader::GraphReader(char * file_name, int bufferSize)
{
    this->total_read = 0;
    this->total_write = 0;

    this->createNodeHash = createNodeHash;
    this->FR = new FileReader(file_name);

    //Save some extra buffer space for partial reads
    uint32 extra_buffer = sizeof(HeaderGraph);

    this->alloc_start = new char[bufferSize*_1_MB + extra_buffer];

    this->buffer_start = this->alloc_start + extra_buffer;
    this->buffer_end = this->buffer_start + bufferSize*_1_MB;

    this->start = this->buffer_start;
    this->end = this->buffer_start;
    this->async = true;
}

GraphReader::GraphReader()
{
    this->total_read = 0;
    this->total_write = 0;
}

GraphReader::~GraphReader()
{
    delete[] this->alloc_start;
    delete this->FR;
}

bool GraphReader::has_next()
{
    return this->FR->has_next() ? true : this->start < this->end;
}

HeaderGraph GraphReader::currentHeader()
{
    uint32 sizeToRead = sizeof(HeaderGraph);

    if (this->start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->start;
        char* dst = this->buffer_start - bytesToCopy;
        memmove(dst, this->start, bytesToCopy);
        this->start = dst;
        this->load();
    }

    return *(HeaderGraph*) this->start;
}


uint32 GraphReader::currentNeighbour()
{
    uint32 sizeToRead = sizeof(uint32);

    if (this->start + sizeToRead > this->end)
    {
        uint32 bytesToCopy = this->end - this->start;
        char* dst = this->buffer_start - bytesToCopy;
        memmove(dst, this->start, bytesToCopy);
        this->start = dst;
        this->load();
    }

    return *(uint32*) this->start;
}


void GraphReader::nextHeader()
{
    this->start += sizeof(HeaderGraph);
}


void GraphReader::nextNeighbour()
{
    this->start += sizeof(uint32);
}


uint32 GraphReader::remainingBuffer()
{
    return (this->end - this->start);
}


void GraphReader::load()
{
    uint32 bytesTransferred = 0;

    if (this->async) {
        this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    }
    else {
        this->FR->read(this->buffer_start, this->buffer_end - this->buffer_start, bytesTransferred);
    }

    this->end = this->buffer_start + bytesTransferred;
    this->total_read += bytesTransferred;
}


void GraphReader::copyRange(char *& dst)
{
    HeaderGraph h = this->currentHeader();
    uint32 sizeToCopy = h.len*sizeof(uint32);
    this->nextHeader();
    if (this->start + sizeToCopy > this->end)
    {
        uint32 partialSize = this->end - this->start;

        memmove(dst, this->start, partialSize);
        dst += partialSize;
        sizeToCopy -= partialSize;
        this->load();
        this->start = this->buffer_start;
    }

    memmove(dst, this->start, sizeToCopy);
    dst += sizeToCopy;
    this->start += sizeToCopy;
}