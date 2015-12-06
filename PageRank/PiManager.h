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

    int prev() { return this->state; }
    int current() { return this->state ^ 1; }

    void invert() { this->state ^= 1; }
    void reset();
    void print();
    void normalize();
    void getTopN();
    void resizeCurrent();
};

