#include "kernighanLin.h"
#include "utils.h"
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_ITERATIONS 10

static int calculateGain(Graph* graph, int* assignment, int u, int v) {
    int gain = 0;
    Node* nu = graph->vertexData[u].neighborsHead;
    while (nu) {
        if (assignment[nu->vertex] == assignment[u]) gain--;
        else gain++;
        nu = nu->next;
    }
    Node* nv = graph->vertexData[v].neighborsHead;
    while (nv) {
        if (assignment[nv->vertex] == assignment[v]) gain--;
        else gain++;
        nv = nv->next;
    }
    return gain;
}

void kernighanLinRefinement(Graph* graph, int* assignment, int parts) {
    int numVert = graph->numVert;
    bool* locked = malloc(sizeof(bool) * numVert);
    if (!locked) handleError(5533, "Memory allocation failed in KL refinement");

    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        for (int i = 0; i < numVert; i++) locked[i] = false;

        int bestGain = 0, bestU = -1, bestV = -1;
        
        for (int g1 = 0; g1 < parts; g1++) {
            for (int g2 = g1 + 1; g2 < parts; g2++) {
                for (int u = 0; u < numVert; u++) {
                    if (assignment[u] != g1 || locked[u]) continue;
                    for (int v = 0; v < numVert; v++) {
                        if (assignment[v] != g2 || locked[v]) continue;
                        int gain = calculateGain(graph, assignment, u, v);
                        if (gain > bestGain) {
                            bestGain = gain;
                            bestU = u;
                            bestV = v;
                        }
                    }
                }
            }
        }

        if (bestGain <= 0) break;

        int temp = assignment[bestU];
        assignment[bestU] = assignment[bestV];
        assignment[bestV] = temp;

        locked[bestU] = true;
        locked[bestV] = true;
    }

    free(locked);
}