#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct VertexInfo { // New struct per vertex
    int vertexId; // Optional, can be inferred from array index
    int row;
    int col;
    Node* neighborsHead;
} VertexInfo;

typedef struct Graph {
    int numVert;
    int maxDim;
    VertexInfo* vertexData;
} Graph;

Graph* allocateGraph(int numVertices, int maxDimension);
void addEdge(Graph* graph, int src, int dest);
Graph* loadGraph(const char *filename, int graphIndex);
void freeGraph(Graph *graph);
void printGraph(const Graph *graph);
int parseLine(char* line, int** arrPtr, int* countPtr);
int failParse(int** arr, int*** arrPtr, int** countPtr);
Graph* failGraph(FILE *file, char* lineBuffer, Graph* graph, int* colIndices, int* rowPointers, int* edgeListIndices, int* edgeGroupPointers);

#endif