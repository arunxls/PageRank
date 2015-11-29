#include "stdafx.h"
#include "GraphWriter.h"


GraphWriter::GraphWriter()
{
}

GraphWriter::GraphWriter(char * file_name)
{
    this->total_read = 0;
    this->total_write = 0;

    this->FW = new FileWriter(file_name);
    this->buffer_start = new char[GRAPH_WRITER_SIZE*_1_MB];
    this->buffer_end = this->buffer_start + GRAPH_WRITER_SIZE*_1_MB;
    
    this->start = (uint32*) this->buffer_start;
    this->end = this->start;
    
    this->truncatedPointer = this->start;
    this->truncatedCounter = 0;
}


GraphWriter::~GraphWriter()
{
    delete[] this->buffer_start;
    delete this->FW;
}

void GraphWriter::writeToDisk()
{
    *this->truncatedPointer = this->truncatedCounter;
    
    this->FW->write(this->buffer_start, (char*) this->start - this->buffer_start);
    
    this->start = (uint32*) this->buffer_start;
}

void GraphWriter::writeHeader(uint32 hash, uint32 len)
{
    *this->truncatedPointer = this->truncatedCounter;

    if ((char*)(this->start + len + 3) > this->buffer_end)
    {
        this->writeToDisk();
    }
    
    this->truncatedCounter = 0;

    *((uint32*)this->start) = hash;
    this->start++;

    *((uint32*)this->start) = len;
    this->start++;

    this->truncatedPointer = this->start;
    this->start++;
}

void GraphWriter::writeTruncated(uint32 node)
{
    *this->start = node;
    this->start++;
    this->truncatedCounter++;
}
