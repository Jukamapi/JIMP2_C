#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "readFile.h"
#include "partition.h"
#include "utils.h"

bool verboseMode = true;

void testLoadGraph() {
    Graph* g = loadGraph("graphs/testGraph.csrrg", 1);
    assert(g != NULL);
    assert(g->numVert > 0);
    freeGraph(g);
    printf("testLoadGraph passed\n");
}

void testPartitionBalance() {
    Graph *g = loadGraph("graphs/testGraph.csrrg", 1);
    assert(g != NULL);

    int* assignment = createAssignmentArray(g->numVert);
    cutGraph(g, 2, 10.0, assignment);
    assert(checkBalance(g, assignment, 2, 10.0));

    free(assignment);
    freeGraph(g);
    printf("testPartitionBalance passed\n");
}

void runTests() {
    testLoadGraph();
    testPartitionBalance();
    printf("All tests passed.\n");
}

int main() {
    runTests();
    return 0;
}