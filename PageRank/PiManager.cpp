#include "stdafx.h"
#include "PiManager.h"


PiManager::PiManager()
{
}

PiManager::PiManager(uint32 ways, uint32 size)
{
    this->ways = ways;
    this->way = new float*[ways];
    this->state = 0;
    this->size = size;

    for (uint32 i = 0; i < ways; ++i) 
    {
        *(this->way + i) = new float[size];
    }
}

PiManager::~PiManager()
{
    for (int i = 0; i < this->ways; ++i)
    {
        delete[] *(this->way + i);
    }

    delete[] this->way;
}