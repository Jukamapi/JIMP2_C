#ifndef READFILE_H
#define READFILE_H

#include "graph.h"

//Graph -> arr of vertices -> list of neighbours to the vertex

Graph* allocateGraph(int numVertices, int maxDimension);
void addEdgeRead(Graph* graph, int src, int dest);
Graph* loadGraph(const char* filename, int graphIndex);
void freeGraphRead(Graph* graph);
void printGraphRead(const Graph* graph);
int parseLine(char* line, int** arrPtr, int* countPtr);
int failParse(int** arr, int*** arrPtr, int** countPtr);
Graph* failGraph(FILE* file, char* lineBuffer, Graph* graph, int* colIndices, int* rowPointers, int* edgeListIndices, int* edgeGroupPointers);

#endif