#ifndef WRITEFILE_H
#define WRITEFILE_H

typedef struct Graph Graph;

int cleanupError(FILE *file, int* rowPointers, int* groupedNodeIndices, int* groupPointers);
int saveGraph(const Graph* graph, const char* filename);


#endif