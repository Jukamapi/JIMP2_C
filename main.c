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
    }

    // Executing the cut
    printf("Info: Cutting graph into %d parts with %.2f%% margin using algorithm %d\n",
           flags.parts, flags.margin, flags.algorithm);

    if (flags.measure) {
        double start = measureTime();
        cutGraph(myGraph, flags.parts, flags.margin, flags.algorithm);
        double end = measureTime();
        printf("Time: %.6f seconds\n", end - start);
    } else {
        cutGraph(myGraph, flags.parts, flags.margin, flags.algorithm);
    }

    // Implementing save path
    char saveFilename[256];
    if (flags.outputFile) {
        strncpy(saveFilename, flags.outputFile, sizeof(saveFilename) - 1);
        saveFilename[sizeof(saveFilename) - 1] = '\0';
    } else {
        snprintf(saveFilename, sizeof(saveFilename), "savedGraphs/savedGraph%d.csrrg", flags.graphIndex);
    }

    printf("\nInfo: Attempting to save graph %d to: %s...\n", flags.graphIndex, saveFilename);
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
