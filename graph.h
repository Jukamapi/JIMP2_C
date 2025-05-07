#ifndef GRAPH_H
#define GRAPH_H

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct VertexInfo {
    int vertexId;
    int row;
    int col;
    Node* neighborsHead;
} VertexInfo;

typedef struct Graph {
    int numVert;
    Node** list;
    int maxDim;
    VertexInfo *vertexData;
} Graph;

// Creates an empty graph with a given size
Graph* createGraph(int numVert);

// Adds an undirected edge
void addEdge(Graph* graph, int src, int dest);

// Frees graph memory up
void freeGraph(Graph* graph);

// Prints graph onto stdout (used for debugging)
void printGraph(const Graph* graph);

#endif
