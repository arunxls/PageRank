#pragma once

#include "GraphReader.h"
#include "PiManager.h"
#include "include.h"
#include <vector>

#define THREADS 2

static const HANDLE gGraph_EMPTY = CreateSemaphore(NULL, 0, THREADS, NULL);
static const HANDLE gGraph_STARTED = CreateSemaphore(NULL, 0, THREADS, NULL);

static CONDITION_VARIABLE GRAPH_LOADED;
static CRITICAL_SECTION   GRAPH_LOCK;

class Graph
{
public:
    PiManager* pi;
    GraphReader* graph;

    uint64 total_read;
    uint64 total_write;

    Graph(GraphReader* graph, PiManager* pi);
    Graph();
    ~Graph();

    char* execute_first();
    void execute_iteration(uint32 num);
    void execute_last();
private:
    HANDLE  hThreadArray[THREADS];
};