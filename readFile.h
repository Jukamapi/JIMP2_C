#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct Graph {
    int numVert;
    struct Node** list;
} Graph;

Graph* allocateGraph(int numVertices);
void addEdge(Graph* graph, int src, int dest);
Graph* loadGraph(const char *filename, int graphIndex);
void freeGraph(Graph *graph);
void printGraph(const Graph *graph);
int parseLine(char* line, int** arrPtr, int* countPtr);
int failParse(int** arr, int*** arrPtr, int** countPtr);
Graph* failGraph(FILE *file, char* lineBuffer, Graph* graph, int* colIndices, int* rowPointers, int* edgeListIndices, int* edgeGroupPointers);
