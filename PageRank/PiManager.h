#pragma once

#include "include.h"
#include <iostream>

class PiManager
{
public:
    float** way;
    uint32 ways;
    uint32 size;

    inline PiManager(uint32 ways, uint32 size)
    {
        this->ways = ways;
        this->way = new float*[ways];

        this->prev = 0;
        this->current = 1;

        this->size = size;

        for (uint32 i = 0; i < ways; ++i)
        {
            *(this->way + i) = new float[size];
        }
    }

    inline ~PiManager()
    {
        for (int i = 0; i < this->ways; ++i)
        {
            delete[] * (this->way + i);
        }

        delete[] this->way;
    }

    short prev;
    short current;

    inline void invert() 
    {
        this->prev ^= 1;
        this->current ^= 1;
    }

    inline void reset() 
    {
        memset(this->way[this->current], 0.0, sizeof(float) * this->size);
    }

    inline void print() 
    {
        double X = 0.0;
        for (int i = 0; i < this->size; ++i)
        {
            X += this->way[this->current][i];
        }

        printf("Current pi sum = %.3f\n", X);
    }

    inline void normalize() 
    {
        double X = 0.0;
        double Y = 1.0;
        for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
        {
            X += this->way[this->prev][i];
        }

        Y = 1.0 - X;

        X *= ((1.0 - ALPHA) / (float)TOTAL_NODES);
        Y *= (1.0 / (float)TOTAL_NODES);

        for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
        {
            this->way[this->current][i] = (ALPHA * this->way[this->current][i]) + X + Y;
        }
    }

    inline void resizeCurrent() 
    {
        delete[] * (this->way + (this->current));
        *(this->way + (this->current)) = new float[TOTAL_NODES];
        this->size = TOTAL_NODES;
    }
};

