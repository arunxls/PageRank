#include "stdafx.h"
#include "TopN.h"
#include "FileWriter.h"
#include <stdio.h>

TopN::TopN()
{
    this->buffer_start = new char[OUTPUT_RANK * _1_MB];
    this->buffer_end = this->buffer_start + OUTPUT_RANK * _1_MB;

    this->start = this->buffer_start;
    this->end = this->start;

    this->total_read = 0;
    this->total_write = 0;
}

TopN::~TopN()
{
}

void TopN::write()
{
    FileWriter FW("output.txt");
    FW.write(this->buffer_start, this->start - this->buffer_start);
}

void TopN::put(uint32 index, uint32 hash, float value)
{
    this->start += sprintf(this->start, "%3d  %8d %f\r\n", index, hash, value);;
}
