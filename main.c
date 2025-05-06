#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "readFile.h"
#include "writeFile.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <filename> <graphIndex>\n", argv[0]);
        fprintf(stderr, "<filename>: Path to the graph file\n");
        fprintf(stderr, "<graphIndex>: Integer index of a graph to load\n");
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


    printf("Info: Attempting to load graph %d from: %s\n", targetIndex, filename);
    Graph *myGraph = loadGraph(filename, targetIndex);

    if (myGraph)
    {
        printf("Info: Successfully loaded graph %d from %s\n", targetIndex, filename);
        printGraph(myGraph);

        // Algorithms should probably go here

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