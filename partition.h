#ifndef PARTITION_H
#define PARTITION_H

#include "graph.h"

// type which can determine what algorithm to use
typedef enum {
    KERNIGHAN_LIN,
    DIJKSTRA_BASED
} AlgorithmType;

void cutGraph(Graph *graph, int parts, double margin, AlgorithmType algorithm);

#endif
