#include "stdafx.h"
#include "Graph.h"


Graph::Graph()
{
    this->total_read = 0;
    this->total_write = 0;
}


Graph::Graph(GraphReader & graph, PiManager & pi)
{
    this->total_read = 0;
    this->total_write = 0;

    this->graph = graph;
    this->pi = pi;
}

Graph::~Graph()
{
}

char * Graph::execute_first()
{
    return nullptr;
}

void Graph::execute_iteration(uint32 num)
{
}

void Graph::execute_last()
{
}
