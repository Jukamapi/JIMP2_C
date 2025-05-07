#ifndef WRITEFILE_H
#define WRITEFILE_H

#include <stdio.h>
#include <stdint.h>

typedef struct Graph Graph;

int cleanupError(FILE *file, int* rowPointers, int* groupedNodeIndices, int* groupPointers);
int saveGraph(const Graph* graph, const char* filename, int binaryFormat);
int writeUint32BigEndian(uint32_t value, FILE* file);
int writeCountAndArrayDeltas11Bit(const int* array, int count, FILE* file);
int writeArrayDataDeltas11Bit(const int* array, int count, FILE* file);
int flushBuffer(FILE* file);
int writeSigned11Bit(int value, FILE* file);
int writeByte(unsigned char byte, FILE* file);



#endif