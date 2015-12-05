#pragma once

#include "include.h"

class PiManager
{
public:
    float** way;
    uint32 ways;
    uint32 state;
    uint32 size;

    PiManager();
    PiManager(uint32 ways, uint32 size);
    ~PiManager();

    int prev() { return state; }
    int current() { return state ^ 1; }

    void invert() { this->state ^= 1; }
    void reset() { memset(this->way[this->current()], 0.0, sizeof(float) * this->size); }
    void print();
    void normalize();
};

