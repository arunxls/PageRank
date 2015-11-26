#pragma once

#include "include.h"

class PiManager
{
public:
    float** way;
    uint32 ways;

    PiManager();
    PiManager(uint32 ways, uint32 size);
    ~PiManager();

};

