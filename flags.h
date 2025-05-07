#ifndef FLAGS_H
#define FLAGS_H

#include "partition.h"

typedef struct {
    const char *inputFile;
    const char *outputFile;
    int binaryFormat;
    int force;
    int verbose;
    int count;
    int measure;
    int graphIndex;
    int parts;
    double margin;
    AlgorithmType algorithm;
    int valid;
} ProgramFlags;

ProgramFlags handleFlags(int argc, char *argv[]);

#endif
