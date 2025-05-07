#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

Graph* createGraph(int numVert) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->numVert = numVert;
    graph->list = malloc(numVert * sizeof(Node*));

    if (!graph->list) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < numVert; i++) {
        graph->list[i] = NULL;
    }

    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
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
    for (int i = 0; i < graph->numVert; i++) {
        Node* current = graph->list[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->list);
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
