#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "readFile.h"
#include "writeFile.h"
#include "flags.h"
#include "graph.h"
#include "utils.h"
#include "partition.h"
#include "test.h"


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <filename> <graphIndex>\n", argv[0]);
        fprintf(stderr, "<filename>: Path to the graph file\n");
        fprintf(stderr, "<graphIndex>: Integer index of a graph to load\n");
        fprintf(stderr, "Optional parameters: \n");
        fprintf(stderr, "   -algorithm <algorithm_name>: Algorithm to use for graph partitioning (e.g., DIJKSTRA, KERNIGHAN_LIN)\n");
        return 1;
    }

    const char *filename = argv[1];
    long val;
    int targetIndex;

    char *endptr;
    val = strtol(argv[2], &endptr, 10);

    if (endptr == argv[2])
    {
        fprintf(stderr, "Error: Graph index argument '%s' is not a number\n", argv[2]);
        return 1;
    }
    // Also checks if i can convert
    if (val <= 0 || val > INT_MAX)
    {
         fprintf(stderr, "Error: Graph index %ld is out of range\n", val);
         return 1;
    }
    targetIndex = (int)val;

    // default algorithm
    AlgorithmType algorithm = DIJKSTRA_BASED;

    // parsing optional arguments
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-algorithm") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "DIJKSTRA") == 0)
            {
                algorithm = DIJKSTRA_BASED;
            }
            else if (strcmp(argv[i], "KERNIGHAN_LIN") == 0)
            {
                algorithm = KERNIGHAN_LIN;
            }
            else
            {
                fprintf(stderr, "Error: Unknown algorithm '%s'\n", argv[i]);
                return 1;
            }
        }
        else
        {
            fprintf(stderr, "Error: Unknown parameter '%s'\n", argv[i]);
            return 1;
        }
    }

    printf("Info: Attempting to load graph %d from: %s\n", targetIndex, filename);
    Graph *myGraph = loadGraph(filename, targetIndex);

    if (myGraph)
    {
        printf("Info: Successfully loaded graph %d from %s\n", targetIndex, filename);
        printGraph(myGraph);

        // Perform graph partitioning
        cutGraph(myGraph, 2, 0.0, algorithm); // Assuming 2 parts and 0% margin for now

        // Writing to a file
        char saveFilename[256];
        snprintf(saveFilename, sizeof(saveFilename), "savedGraphs/savedGraph%d.csrrg", targetIndex);

        printf("\nInfo: Attempting to save graph %d to: %s...\n", targetIndex, saveFilename);
        int saveStatus = saveGraph(myGraph, saveFilename);

        if (saveStatus == 0)
        {
            printf("Info: Graph saved successfully to %s.\n", saveFilename);
        }
        else
        {
            fprintf(stderr, "Error: Failed to save graph to %s.\n", saveFilename);
        }

        freeGraph(myGraph);
        printf("Info: Graph memory freed.\n");
    }
    else
    {
        fprintf(stderr, "Error: Failed to load graph %d from %s\n", targetIndex, filename);
        return 1;
    }

    return 0;
}