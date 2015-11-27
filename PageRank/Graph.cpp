#include "stdafx.h"
#include "Graph.h"
#include <algorithm>
#include <ppl.h>

Graph::Graph()
{
    this->total_read = 0;
    this->total_write = 0;
}


Graph::Graph(GraphReader* graph, PiManager* pi)
{
    this->total_read = 0;
    this->total_write = 0;

    this->graph = graph;
    this->pi = pi;

    InitializeCriticalSection(&GRAPH_LOCK);
    InitializeConditionVariable(&GRAPH_LOADED);
}

Graph::~Graph()
{
}

DWORD WINAPI threadFirst(LPVOID data)
{
    std::pair < std::pair<int, int>, Graph* > arg = *(std::pair < std::pair<int, int>, Graph* > *) data;

    GraphReader graph = *arg.second->graph;
    PiManager* pi = arg.second->pi;
    int lower = arg.first.first;
    int upper = arg.first.second;

    float offset = 1.0 / TOTAL_NODES;

    while (1) 
    {
        uint32 currentHash = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        //printf("%I32u\n", currentHash);

        uint32 currentLen = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        uint32 count = currentLen;

        while (--count != -1)
        {
            uint32 node = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
            if (node >= lower && node <= upper)
            {
                pi->way[pi->current()][node] += (offset / currentLen);
            }
        }
    }
}

char * Graph::execute_first()
{
    this->pi->reset();
    this->graph->load();

    DWORD   dwThreadIdArray[THREADS];

    //Create args
    uint32 offset = TOTAL_NODES_WITH_OUT_DEGREE / THREADS;
    std::vector<std::pair < std::pair<int, int>, Graph* >> args = this->create_args(offset);

    //Threads get started here!
    for (int i = 0; i < THREADS; ++i) 
    {
        this->hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            threadFirst,            // thread function name
            &args[i],               // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 


        if (DEBUG && hThreadArray[i] == NULL)
        {
            //ErrorHandler(TEXT("CreateThread"));
            ExitProcess(3);
        }
    }

    while (this->graph->FR->has_next())
    {
        for (int i = 0; i < THREADS; ++i)
        {
            WaitForSingleObject(gGraph_EMPTY, INFINITE);
        }

        this->graph->load();
        WakeAllConditionVariable(&GRAPH_LOADED);
    }

    for (int i = 0; i < THREADS; ++i)
    {
        WaitForSingleObject(gGraph_EMPTY, INFINITE);
    }

    printf("DONE!\n");

    return nullptr;
}

void Graph::execute_iteration(uint32 num)
{
}

void Graph::execute_last()
{
}

std::vector<std::pair<std::pair<int, int>, Graph*>> Graph::create_args(uint32 offset)
{
    std::vector<std::pair < std::pair<int, int>, Graph* >> args;
    {
        std::pair<int, int> bounds(0, offset);
        args.push_back(std::make_pair(bounds, this));
    }

    for (int i = 1; i < THREADS - 1; ++i)
    {
        std::pair<int, int> bounds(args[i - 1].first.second + 1, args[i - 1].first.second + 1 + offset);
        args.push_back(std::make_pair(bounds, this));
    }

    {
        std::pair<int, int> bounds(args[THREADS - 2].first.second + 1, TOTAL_NODES_WITH_OUT_DEGREE);
        args.push_back(std::make_pair(bounds, this));
    }

    return args;
}
