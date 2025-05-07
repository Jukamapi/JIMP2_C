#include <stdio.h>
#include <stdlib.h>
#include "partition.h"
#include "dijkstra.h"
#include "kernighan_lin.h"
#include "graph.h"

void cutGraph(Graph *graph, int parts, double margin, AlgorithmType algorithm) {
    int numVertices = graph->numVert;

    switch (algorithm) {
        case DIJKSTRA_BASED: {
            printf("[INFO] Using Dijkstra-based partitioning\n");

            int *distances = malloc(sizeof(int) * numVertices);
            int *predecessors = malloc(sizeof(int) * numVertices);
            int *partition = malloc(sizeof(int) * numVertices);

            if (!distances || !predecessors || !partition) {
                fprintf(stderr, "[ERROR] Allocation failed in cutGraph\n");
                exit(1);
            }

            // Starting Dijkstra from node 0
            dijkstra(graph, 0, distances, predecessors);

            // Grouping based on distance: modulo of the number of parts
            for (int i = 0; i < numVertices; i++) {
                if (distances[i] == __INT_MAX__) {
                    partition[i] = -1; // unavailable
                } else {
                    partition[i] = distances[i] % parts;
                }
            }

            // Printing out partition (for testing)
            for (int i = 0; i < numVertices; i++) {
                printf("Vertex %d => part %d (dist: %d)\n", i, partition[i], distances[i]);
            }

            free(distances);
            free(predecessors);
            free(partition);
            break;
        }

        case KERNIGHAN_LIN: {
            printf("[INFO] Using Kernighan-Lin partitioning\n");

            int *partition = malloc(sizeof(int) * numVertices);
            if (!partition) {
                fprintf(stderr, "[ERROR] Allocation failed in cutGraph\n");
                exit(1);
            }

            kernighanLin(graph, partition);

            for (int i = 0; i < numVertices; i++) {
                printf("Vertex %d => part %d\n", i, partition[i]);
            }

            free(partition);
            break;
        }
        default:
            fprintf(stderr, "[ERROR] Unknown algorithm type.\n");
            break;
    }
}
