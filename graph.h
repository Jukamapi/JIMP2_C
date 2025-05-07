#ifndef GRAPH_H
#define GRAPH_H

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct Graph {
    int numVert;
    Node** list;
} Graph;

// Creates an empty graph with a given size
Graph* createGraph(int numVert);

// Adds an undirected edge
void addEdge(Graph* graph, int src, int dest);

// Frees graph memory up
void freeGraph(Graph* graph);

// Prints graph onto stdout (used for debugging)
void printGraph(Graph* graph);

#endif
