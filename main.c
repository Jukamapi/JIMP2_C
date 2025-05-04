#include <stdio.h>

#include "readFile.h"

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
    int target_index;

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
    target_index = (int)val;


    printf("Info: Attempting to load graph %d from: %s\n", target_index, filename);
    Graph *myGraph = loadGraph(filename, target_index);

    if (myGraph)
    {
        printf("Info: Successfully loaded graph %d from %s\n", target_index, filename);
        printGraph(myGraph);

        // Place for calling algorithms

        freeGraph(myGraph);
        printf("Info: Graph memory freed.\n");
    }
    else
    {
        fprintf(stderr, "Error: Failed to load graph %d from %s\n", target_index, filename);
        return 1;
    }

    return 0;
}