#include "stdafx.h"
#include "Graph.h"
#include "GraphWriter.h"
#include "utils.h"
#include <utility>
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

DWORD WINAPI threadSecond(LPVOID data)
{
    std::pair < std::pair<int, int>, Graph* > arg = *(std::pair < std::pair<int, int>, Graph* > *) data;

    GraphReader graph = *arg.second->graph;
    PiManager* pi = arg.second->pi;
    int lower = arg.first.first;
    int upper = arg.first.second;

    while (1) 
    {
        uint32 currentHash = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        //printf("%I32u\n", currentHash);

        uint32 currentLen = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        uint32 count = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);

        while (--count != -1)
        {
            uint32 node = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
            if (node >= lower && node <= upper)
            {
                pi->way[pi->current()][node] += (pi->way[pi->prev()][currentHash] / currentLen);
            }
        }
    }
}

char * Graph::execute_first()
{
    this->pi->reset();
    this->graph->reset();

    this->graph->load();

    //GraphWriter writer(getNewOutputFile());

    float offset = 1.0 / TOTAL_NODES;
    
    while (this->graph->has_next())
    {
        uint32 currentHash = this->graph->next_node();
        //printf("%I32u\n", currentHash);

        uint32 currentLen = this->graph->next_node();
        uint32 count = currentLen;

        //writer.writeHeader(currentHash, currentLen);

        while (--count != -1)
        {
            uint32 node = this->graph->next_node();
            if (node <= TOTAL_NODES_WITH_OUT_DEGREE)
            {
                //writer.writeTruncated(node);
                this->pi->way[this->pi->current()][node] += (offset / currentLen);
            }
        }
    }

    double X = offset * TOTAL_NODES_WITH_OUT_DEGREE;
    double Y = 1.0 - X;

    X *= (1.0 - ALPHA) / (float) this->pi->size;
    Y *= 1.0 / (float) this->pi->size;

    for (int i = 0; i < this->pi->size; ++i)
    {
        this->pi->way[this->pi->current()][i] = (ALPHA * this->pi->way[this->pi->current()][i]) + X + Y;
    }
    
    this->pi->print();
    

    //writer.writeToDisk();

    //return writer.FW->filename;
    return new char[1];
}

void Graph::execute_iteration(uint32 num)
{
    this->graph->reset();

    //Create args
    uint32 offset = TOTAL_NODES_WITH_OUT_DEGREE / THREADS;
    std::vector<std::pair < std::pair<int, int>, Graph* >> args = this->create_args(offset);

    //Threads get started here!
    DWORD   dwThreadIdArray[THREADS];
    for (int i = 0; i < THREADS; ++i)
    {
        this->hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            threadSecond,            // thread function name
            &args[i],               // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 


        if (DEBUG && hThreadArray[i] == NULL)
        {
            //ErrorHandler(TEXT("CreateThread"));
            ExitProcess(3);
        }
    }

    for (uint32 i = 0; i < num; ++i)
    {
        //Re-init for each iteration.
        this->pi->reset();
        this->graph->reset();

        //Start each iteration here
        this->graph->load();
        WakeAllConditionVariable(&GRAPH_LOADED);

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
        
        printf("%I32u iteration done!\n", i);
        this->pi->invert();
    }

    //Close threads here
    for (int i = 0; i < THREADS; ++i) {
        CloseHandle(this->hThreadArray[i]);
    }

    printf("DONE!");
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
