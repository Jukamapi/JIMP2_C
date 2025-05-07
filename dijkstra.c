#include "dijkstra.h"
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

void dijkstra(Graph *graph, int startVertex, int *distances, int *predecessors) {
    int numVertices = graph->numVert;
    bool *visited = calloc(numVertices, sizeof(bool));

    for (int i = 0; i < numVertices; i++) {
        distances[i] = INT_MAX;
        predecessors[i] = -1;
    }

    distances[startVertex] = 0;

    for (int i = 0; i < numVertices - 1; i++) {
        int minDist = INT_MAX;
        int u = -1;

        for (int v = 0; v < numVertices; v++) {
            if (!visited[v] && distances[v] < minDist) {
                minDist = distances[v];
                u = v;
            }
        }

        if (u == -1) break;
        visited[u] = true;

        Node *neighbor = graph->list[u];
        while (neighbor != NULL) {
            int v = neighbor->vertex;
            if (!visited[v] && distances[u] != INT_MAX && distances[u] + 1 < distances[v]) {
                distances[v] = distances[u] + 1;
                predecessors[v] = u;
            }
            neighbor = neighbor->next;
        }
    }

    free(visited);
}
