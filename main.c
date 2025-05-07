// Note: Graphs 2-5 are not fully loading

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
    ProgramFlags flags = handleFlags(argc, argv);
    if (!flags.valid) {
        return 1;
    }

    if (flags.graphIndex <= 0 || flags.graphIndex > INT_MAX) {
        fprintf(stderr, "Error: Graph index %d is out of range\n", flags.graphIndex);
        return 1;
    }

    printf("Info: Attempting to load graph %d from: %s\n", flags.graphIndex, flags.inputFile);
    Graph *myGraph = loadGraph(flags.inputFile, flags.graphIndex);

    if (!myGraph) {
        fprintf(stderr, "Error: Failed to load graph %d from %s\n", flags.graphIndex, flags.inputFile);
        return 1;
    }

    printf("Info: Successfully loaded graph %d from %s\n", flags.graphIndex, flags.inputFile);
    if (flags.verbose) {
        printGraph(myGraph);

        // Algorithms should probably go here

        // Writing to a file
        char saveFilename[256];
        snprintf(saveFilename, sizeof(saveFilename), "savedGraphs/savedGraph%d.csrrg", targetIndex);

        printf("\nInfo: Attempting to save graph %d to: %s...\n", targetIndex, saveFilename);
        int saveStatus = saveGraph(myGraph, saveFilename);

    if (saveStatus == 0) {
        printf("Info: Graph saved successfully to %s.\n", saveFilename);
    } else {
        fprintf(stderr, "Error: Failed to save graph to %s.\n", saveFilename);
    }

    if (flags.count) {
        printf("Note: --count flag was provided (counting not yet implemented).\n");
        // to improve - WR
    }

    freeGraph(myGraph);
    printf("Info: Graph memory freed.\n");

    return 0;
}
