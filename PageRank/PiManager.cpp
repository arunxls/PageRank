#include "stdafx.h"
#include "PiManager.h"


PiManager::PiManager()
{
}

PiManager::PiManager(uint32 ways, uint32 size)
{
    this->ways = ways;
    this->way = new float*[ways];

    for (int i = 0; i < ways; ++i) 
    {
        *(this->way + i) = new float[size];
    }
}


PiManager::~PiManager()
{
}