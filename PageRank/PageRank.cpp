// PageRank.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <chrono>
#include "include.h"
#include <psapi.h>
#include "PiManager.h"
#include "Graph.h"

//no except
void __cdecl _tmain(int argc, TCHAR *argv[]) noexcept
{
    if (argc != 2)
    {
        printf("Usage Error: Incorrect number of arguments\n\n");
        _tprintf("Usage:\n\t%s <graph_file_name>\n", argv[0]);
        return;
    }

    uint64 total_read = 0;
    uint64 total_write = 0;

    uint32 buffer_size = BUFFERSIZE * _1_MB;

    clock_t begin, end;

    printf("Starting pageRank\n");

    std::chrono::high_resolution_clock::time_point b1 = std::chrono::high_resolution_clock::now();
    {
        PiManager pi(2, TOTAL_NODES_WITH_OUT_DEGREE);
        GraphReader reader;

        reader.init(argv[1], false);
        Graph graph(&reader, &pi);
        char* truncated_graph = graph.execute_first();
        
        //printf("Total IO: read - %.2f GB; write - %.2f GB\n", (float)splitHash.total_read / _1_GB, (float)splitHash.total_write / _1_GB);
        //total_read += (float)splitHash.total_read;
        //total_write += (float)splitHash.total_write;
    }
    std::chrono::high_resolution_clock::time_point e1 = std::chrono::high_resolution_clock::now();
    printf("Took %lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count());
    printf("Ending split phase\n");

    printf("\n");

    printf("Overall stats - RunTime: %lld seconds; Total read %.2f GB; Total write %.2f GB\n",
        std::chrono::duration_cast<std::chrono::seconds>(e1 - b1).count(),
        (float)total_read / _1_GB, (float)total_write / _1_GB);

    {
        PROCESS_MEMORY_COUNTERS pp;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, GetCurrentProcessId());
        GetProcessMemoryInfo(hProcess, &pp, sizeof(PROCESS_MEMORY_COUNTERS));
        printf("Peak working set : %.2f MB\n", (double)pp.PeakWorkingSetSize / _1_MB);
    }

    printf("DONE!\n");
    return;
}

