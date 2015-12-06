#include "stdafx.h"
#include "Graph.h"
#include "GraphWriter.h"
#include <utility>
#include <ppl.h>
#include "TopN.h"
#include <deque>
#include <queue>

typedef std::pair<double, uint32> pt;
struct comparator {
    bool operator()(pt& a, pt& b) {
        return a.first > b.first;
    }
};

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

        uint32 currentLen = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        uint32 count = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
        //uint32 count = currentLen;

        while (--count != -1)
        {
            uint32 node = graph.next_node(&gGraph_EMPTY, &GRAPH_LOADED, &GRAPH_LOCK, arg.second->graph);
            if (node >= lower && node <= upper)
            {
                pi->way[pi->current][node] += (pi->way[pi->prev][currentHash] / currentLen);
            }
        }
    }
}

char * Graph::execute_first()
{
    std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();

    this->pi->reset();

    //this->graph->load();

    char* truncated_file = new char[24];
    truncated_file = "PLD-truncated-graph.dat";
    GraphWriter writer(truncated_file);

    float offset = 1.0 / TOTAL_NODES;
    while (this->graph->has_next())
    {
        uint32 currentHash = this->graph->next_node();
        //printf("%I32u\n", currentHash);

        uint32 currentLen = this->graph->next_node();
        uint32 count = currentLen;

        writer.writeHeader(currentHash, currentLen);
        while (--count != -1)
        {
            uint32 node = this->graph->next_node();
            if (node < TOTAL_NODES_WITH_OUT)
            {
                writer.writeTruncated(node);
                this->pi->way[this->pi->current][node] += (offset / currentLen);
            }
        }
    }

    double X = offset * TOTAL_NODES_WITH_OUT;
    double Y = 1.0 - X;

    X *= (1.0 - ALPHA) / (float)TOTAL_NODES;
    Y *= 1.0 / (float)TOTAL_NODES;

    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        this->pi->way[this->pi->current][i] = (ALPHA * this->pi->way[this->pi->current][i]) + X + Y;
    }

    //this->pi->getTopN();
    //this->pi->print();

    writer.writeToDisk();

    std::chrono::high_resolution_clock::time_point e1 = std::chrono::high_resolution_clock::now();
    int time = std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count();
    printf("Done iteration 1 in %lld seconds. Estimated time left: %lld seconds\n", time, time * 9);

    return writer.FW->filename;
}

void Graph::execute_iteration(uint32 num)
{
    this->graph->reset();

    //Create args
    uint32 offset = TOTAL_NODES_WITH_OUT / THREADS;
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
        std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();
        //Re-init for each iteration.
        this->pi->invert();
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

        this->pi->normalize();

        //this->pi->getTopN();
        //this->pi->print();

        std::chrono::high_resolution_clock::time_point e1 = std::chrono::high_resolution_clock::now();
        int time = std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count();
        printf("Done iteration %d in %lld seconds. Estimated time left: %lld seconds\n", i + 2, time, time * (num - i));
    }

    //Close threads here
    for (int i = 0; i < THREADS; ++i) {
        CloseHandle(this->hThreadArray[i]);
    }

    std::priority_queue<std::pair<double, uint32>, std::vector<std::pair<double, uint32>>, comparator> queue;
    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        std::pair<double, uint32>p(this->pi->way[this->pi->current][i], i);
        if (queue.size() == TOPN) {
            std::pair<double, uint32>tmp = queue.top();
            if (tmp.first < p.first) {
                queue.pop();
                queue.emplace(p);
            }
        }
        else {
            queue.emplace(p);
        }
    }

    TopN top = TopN();
    std::deque<std::pair<float, uint32>> r_queue;
    while (queue.size() > 0) {
        r_queue.emplace_back(queue.top());
        queue.pop();
    }

    int index = 0;
    for (auto i = r_queue.rbegin(); i != r_queue.rend(); ++i) {
        top.put(++index, (*i).second, (*i).first);
    }

    top.write();
}

//void Graph::execute_iteration(uint32 num) 
//{
//    for (int i = 0; i < num; ++i) 
//    {
//        this->graph->reset();
//        this->pi->invert();
//        this->pi->reset();
//        
//        this->graph->load();
//        while (this->graph->has_next())
//        {
//            uint32 currentHash = this->graph->next_node();
//
//            uint32 currentLen = this->graph->next_node();
//            //uint32 count = this->graph->next_node();
//            uint32 count = currentLen;
//
//            while (--count != -1)
//            {
//                uint32 node = this->graph->next_node();
//                if (node < TOTAL_NODES_WITH_OUT)
//                {
//                    pi->way[pi->current()][node] += (pi->way[pi->prev()][currentHash] / currentLen);
//                }
//            }
//        }
//
//        this->pi->normalize();
//
//        int array[] = { 24247151,13620962,21153020,22905655,13475170,17991418,1243757,14715032,960765,8269470,21025669 };
//        for (int i = 0; i < 11; ++i) {
//            printf("INDEX %d VALUE %f\n", array[i], this->pi->way[this->pi->current()][array[i]]);
//        }
//
//        this->pi->print();
//        printf("%I32u iteration done!\n", i + 2);
//    }
//}

void Graph::execute_last()
{
    std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();

    this->pi->invert();
    this->pi->resizeCurrent();
    this->pi->reset();

    this->graph->reset();
    this->graph->load();

    while (this->graph->has_next())
    {
        uint32 currentHash = this->graph->next_node();

        uint32 currentLen = this->graph->next_node();
        uint32 count = currentLen;

        while (--count != -1)
        {
            uint32 node = this->graph->next_node();
            this->pi->way[this->pi->current][node] += (pi->way[pi->prev][currentHash] / currentLen);
        }
    }

    double X = 0.0;
    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        X += this->pi->way[this->pi->prev][i];
    }

    double Y = 1.0 - X;

    X *= (1.0 - ALPHA) / (float)TOTAL_NODES;
    Y *= 1.0 / (float)TOTAL_NODES;
    for (int i = 0; i < this->pi->size; ++i)
    {
        this->pi->way[this->pi->current][i] = (ALPHA * this->pi->way[this->pi->current][i]) + X + Y;
    }

    std::chrono::high_resolution_clock::time_point e1 = std::chrono::high_resolution_clock::now();
    int time = std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count();
    printf("Done iteration 10 in %lld seconds.\n", time);

    std::priority_queue<std::pair<double, uint32>, std::vector<std::pair<double, uint32>>, comparator> queue;
    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        std::pair<double, uint32>p(this->pi->way[this->pi->current][i], i);
        if (queue.size() == TOPN) {
            std::pair<double, uint32>tmp = queue.top();
            if (tmp.first < p.first) {
                queue.pop();
                queue.emplace(p);
            }
        }
        else {
            queue.emplace(p);
        }
    }

    TopN top = TopN();
    std::deque<std::pair<float, uint32>> r_queue;
    while (queue.size() > 0) {
        r_queue.emplace_back(queue.top());
        queue.pop();
    }

    int index = 0;
    for (auto i = r_queue.rbegin(); i != r_queue.rend(); ++i) {
        top.put(++index, (*i).second, (*i).first);
    }

    top.write();
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
        std::pair<int, int> bounds(args[THREADS - 2].first.second + 1, TOTAL_NODES_WITH_OUT);
        args.push_back(std::make_pair(bounds, this));
    }

    return args;
}