#pragma once

#include "GraphReader.h"
#include "PiManager.h"
#include "include.h"

class Graph
{
public:
    PiManager pi;
    GraphReader graph;

    uint64 total_read;
    uint64 total_write;

    Graph(GraphReader& graph, PiManager& pi);
    Graph();
    ~Graph();

    char* execute_first();
    void execute_iteration(uint32 num);
    void execute_last();
};

