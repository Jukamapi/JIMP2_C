#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

Graph* createGraph(int numVert) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->numVert = numVert;
    graph->list = calloc(numVert, sizeof(Node*));
    if (!graph->list) {
        free(graph);
        return NULL;
    }

    graph->vertexData = calloc(numVert, sizeof(VertexInfo));
    if (!graph->vertexData) {
        free(graph->list);
        free(graph);
        return NULL;
    }

    for (int i = 0; i < numVert; i++) {
        graph->list[i] = NULL;
    }

    return graph;
}

void addEdge(Graph* graph, int src, int dest) {

    if (!graph) {
    fprintf(stderr, "FATAL: graph is NULL in addEdge\n");
    exit(1);
}
if (!graph->list) {
    fprintf(stderr, "FATAL: graph->list is NULL in addEdge\n");
    exit(1);
}
if (src >= graph->numVert || dest >= graph->numVert || src < 0 || dest < 0) {
    fprintf(stderr, "FATAL: invalid indices: src=%d, dest=%d (numVert=%d)\n", src, dest, graph->numVert);
    exit(1);
}

    printf("DEBUG: adding edge %d <-> %d\n", src, dest);
    if (src < 0 || src >= graph->numVert || dest < 0 || dest >= graph->numVert) 
    {
    fprintf(stderr, "FATAL: invalid edge %d <-> %d (numVert = %d)\n", src, dest, graph->numVert);
    exit(1);
    }

    Node* newNode = malloc(sizeof(Node));
    if (!newNode) return;

    newNode->vertex = dest;
    newNode->next = graph->list[src];
    graph->list[src] = newNode;

    // Adds the opposite edge
    newNode = malloc(sizeof(Node));
    if (!newNode) return;

    newNode->vertex = src;
    newNode->next = graph->list[dest];
    graph->list[dest] = newNode;
}

void freeGraph(Graph* graph) {
    if (!graph) return;

    if (graph->vertexData) {
        for (int i = 0; i < graph->numVert; i++) {
            Node* current = graph->vertexData[i].neighborsHead;
            while (current) {
                Node* temp = current;
                current = current->next;
                free(temp);
            }
        }
        free(graph->vertexData);
    }

    free(graph);
}

void printGraph(const Graph* graph) {
    for (int i = 0; i < graph->numVert; i++) {
        Node* temp = graph->list[i];
        printf("Vertex %d:", i);
        while (temp != NULL) {
            printf(" -> %d", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}
