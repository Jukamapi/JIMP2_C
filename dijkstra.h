#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "readFile.h"

int* computeShortestPaths(Graph* graph, int source);
void findMostDistantPair(Graph* graph, int* seedA, int* seedB);
void assignByDistance(Graph* graph, int* assignment, int seedA, int seedB);
void dijkstraPartition(Graph* graph, int* assignment, int parts);

#endif
