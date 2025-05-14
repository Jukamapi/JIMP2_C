#include <stdlib.h>
#include "partition.h"
#include "utils.h"
#include "kernighanLin.h"
#include "dijkstra.h"

void cutGraph(Graph *graph, int parts, double margin, int* assignment) {
    int attempts = 0;
    const int maxAttempts = 8;
    double currentMargin = margin;

    while (attempts < maxAttempts) {
        dijkstraPartition(graph, assignment, parts);
        kernighanLinRefinement(graph, assignment, parts);

        if (checkBalance(graph, assignment, parts, currentMargin)) {
            break;
        }

        currentMargin += 5.0;
        attempts++;
        fprintf(stderr, "Retrying with increased margin: %.1f%%\n", currentMargin);
    }

    if (attempts == maxAttempts) {
        handleError(31, "Graph division doesn't meet balance criteria after retries");
    }

    if (!graph || parts <=0 || margin < 0) {
        handleError(32, "Invalid arguments in function cutGraph");
    }

    // Additional logging can be added here, e.g. the number of intersections
    int cutEdges = countCutEdges(graph, assignment);
    fprintf(stderr, "Number of intersections: %d\n", cutEdges);

    // Assigning groups to vertices (the Graph structure can be extended if needed)
    for (int i = 0; i < graph->numVert; i++) {
        graph->vertexData[i].groupId = assignment[i];
    }

}