#include <stdio.h>
#include "graph.h"
#include "utils.h"

static void testGraphCreation() {
    printf("[TEST] Creating and printing a simple graph\n");

    Graph *g = createGraph(4);
    if (!g) handleError(11, "Program could not create a graph");

    addEdge(g, 0, 1);
    addEdge(g, 0, 2);
    addEdge(g, 1, 2);
    addEdge(g, 2, 3);

    printGraph(g);

    freeGraph(g);
    printf("[OK] testGraphCreation\n\n");
}

static void testEmptyGraph() {
    printf("[TEST] Empty graph (0 nodes)\n");

    Graph *g = createGraph(0);
    if (!g) handleError(12, "Program could not create an empty graph");

    printGraph(g);
    freeGraph(g);

    printf("[OK] testEmptyGraph\n\n");
}

void runTests() {
    printf("=== STARTING TESTS ===\n\n");
    testGraphCreation();
    testEmptyGraph();
    printf("=== ALL TESTS HAVE BEEN FINISHED ===\n");
}
