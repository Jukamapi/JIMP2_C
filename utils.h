#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "readFile.h"

void handleError(int errorCode, const char *message);
double measureTime();
void printHelp();
int *createAssignmentArray(int numVert);
int checkBalance(Graph *graph, int *assignment, int parts, double margin);
void printAssignmentSummary(Graph *graph, int *assignment, int parts);
int countCutEdges(Graph *graph, int *assignment, int parts);

#endif