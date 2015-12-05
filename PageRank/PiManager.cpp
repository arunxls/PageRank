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

void PiManager::print()
{
    double X = 0.0;
    for (int i = 0; i < this->size; ++i)
    {
        X += this->way[this->current()][i];
    }

    printf("Current pi sum = %.3f\n", X);
}

void PiManager::normalize()
{
    double X = 0.0;
    double Y = 1.0;
    for (int i = 0; i < this->size; ++i)
    {
        X += this->way[this->current()][i];
    }

    Y = 1.0 - X;

    X *= (1.0 - ALPHA) /(float) this->size;
    Y *= 1.0 /(float) this->size;

    for (int i = 0; i < this->size; ++i)
    {
        this->way[this->current()][i] = (ALPHA * this->way[this->current()][i]) + X + Y;
    }
}
