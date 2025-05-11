#include "dijkstra.h"
#include "utils.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct QueueNode {
    int vertex;
    int dist;
    struct QueueNode* next;
} QueueNode;

static void push(QueueNode** head, int vertex, int dist) {
    QueueNode* newNode = malloc(sizeof(QueueNode));
    newNode->vertex = vertex;
    newNode->dist = dist;
    newNode->next = NULL;

    if (!*head || dist < (*head)->dist) {
        newNode->next = *head;
        *head = newNode;
        return;
    }

    QueueNode* current = *head;
    while (current->next && current->next->dist <= dist) current = current->next;
    newNode->next = current->next;
    current->next = newNode;
}

static QueueNode* pop(QueueNode** head) {
    if (!*head) return NULL;
    QueueNode* temp = *head;
    *head = (*head)->next;
    return temp;
}

int* computeShortestPaths(Graph* graph, int source) {
    int numVert = graph->numVert;
    int* dist = malloc(numVert * sizeof(int));
    int *visited = calloc(numVert, sizeof(int));
    if (!dist || !visited) handleError(34441, "Memory allocation failed in computeShortestPaths");

    for (int i = 0; i < numVert; i++) dist[i] = INT_MAX;
    dist[source] = 0;

    QueueNode* pq = NULL;
    push(&pq, source, 0);

    while (pq) {
        QueueNode* node = pop(&pq);
        int u = node->vertex;
        free(node);

        if (visited[u]) continue;
        visited[u] = 1;

        Node* neighbor = graph->vertexData[u].neighborsHead;
        while (neighbor) {
            int v = neighbor->vertex;
            if (!visited[v] && dist[u] + 1 < dist[v]) {
                dist[v] = dist[u] + 1;
                push(&pq, v, dist[v]);
            }
            neighbor = neighbor->next;
        }
    }

    free(visited);
    return dist;
}

void findMostDistantPair(Graph* graph, int* seedA, int* seedB) {
    int numVert = graph->numVert;
    int maxDist = -1;
    *seedA = 0;
    *seedB = 0;

    for (int i = 0; i < numVert; i++) {
        int* dist = computeShortestPaths(graph, i);
        for (int j = 0; j < numVert; j++) {
            if (dist[j] != INT_MAX && dist[j] > maxDist) {
                maxDist = dist[j];
                *seedA = i;
                *seedB = j;
            }
        }
        free(dist);
    }
}

void assignByDistance(Graph* graph, int* assignment, int seedA, int seedB) {
    int* distA = computeShortestPaths(graph, seedA);
    int* distB = computeShortestPaths(graph, seedB);
    int numVert = graph->numVert;

    for (int i = 0; i < numVert; i++) {
        if (distA[i] <= distB[i]) assignment[i] = 0;
        else assignment[i] = 1;
    }

    free(distA);
    free(distB);
}

void dijkstraPartition(Graph* graph, int* assignment, int parts) {
    if (!graph || !assignment || parts < 2) {
        handleError(344441, "Dijkstra partition requires at least 2 parts");
    }

    int numVert = graph->numVert;

    for (int i = 0; i < numVert; i++) {
        assignment[i] = -1;
    }

    int* seeds = malloc(parts * sizeof(int));
    if (!seeds) handleError(344442, "Memory allocation failed for seeds");

    int spacing = numVert / parts;
    for (int i = 0; i < parts; i++) {
        seeds[i] = i * spacing;
        if (seeds[i] >= numVert) seeds[i] = numVert - 1;
    }

    int** distances = malloc(parts * sizeof(int*));
    if (!distances) handleError(344443, "Failed to allocate distances array");

    for (int i = 0; i < parts; i++) {
        distances[i] = computeShortestPaths(graph, seeds[i]);
    }

    for (int v = 0; v < numVert; v++) {
        int bestGroup = -1;
        int bestDist = INT_MAX;

        for (int g = 0; g < parts; g++) {
            if (distances[g][v] < bestDist) {
                bestDist = distances[g][v];
                bestGroup = g;
            }
        }

        assignment[v] = (bestGroup == -1) ? (v % parts) : bestGroup;
    }

    int* counts = calloc(parts, sizeof(int));
    if (!counts) handleError(344444, "Failed to allocate counts");

    for (int i = 0; i < numVert; i++) {
        if (assignment[i] >= 0 && assignment[i] < parts) {
            counts[assignment[i]]++;
        }
    }

    int avg = numVert / parts;

    for (int g = 0; g < parts; g++) {
        while (counts[g] < avg) {
            int maxGroup = 0;
            for (int i = 1; i < parts; i++) {
                if (counts[i] > counts[maxGroup]) {
                    maxGroup = i;
                }
            }

            int moved = 0;
            for (int v = 0; v < numVert; v++) {
                if (assignment[v] == maxGroup) {
                    assignment[v] = g;
                    counts[g]++;
                    counts[maxGroup]--;
                    moved = 1;
                    break;
                }
            }

            if(!moved) break;
        }
    }

    for (int i = 0; i < parts; i++) free(distances[i]);
    free(distances);
    free(seeds);
    free(counts);
}
