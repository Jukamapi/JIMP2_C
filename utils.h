#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

#include "readFile.h"

extern bool verboseMode;

#define LOG_INFO_VERBOSE(format_str, ...) \
    do { \
        if (verboseMode) { \
            printf(format_str, ##__VA_ARGS__); \
        } \
    } while(0)

void handleError(int errorCode, const char *message);
double measureTime();
void printHelp();
int *createAssignmentArray(int numVert);
int checkBalance(Graph *graph, int *assignment, int parts, double margin);
void printAssignmentSummary(Graph *graph, int *assignment, int parts);
int countCutEdges(Graph *graph, int *assignment, int parts);

#endif