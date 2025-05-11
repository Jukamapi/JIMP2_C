#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "utils.h"

void handleError(int errorCode, const char *message) {
    fprintf(stderr, "Error [%d]: %s\n", errorCode, message);
    exit(EXIT_FAILURE);
}

double measureTime() {
   static clock_t start = 0;
   if (start == 0) start = clock();
   return (double)(clock() - start) / CLOCKS_PER_SEC;
}

void printHelp() {
    printf("Uzycie: ./cut [ilosc grup do podzialu] [margines procentowy opisujacy roznice ilosci wezlow] - i [nazwa pliku z grafem]\n");
    printf("Parametry:\n");
    printf("  -help                  Wyswietla instrukcje wywolania programu oraz jego argumenty wywolania\n");
    printf("  -input <nazwa pliku>   Okresla plik wejsciowy, z ktorego program ma wczytac dane\n");
    printf("  -output <nazwa pliku>  Okresla plik wyjsciowy, do ktorego program zapisze wyniki\n");
    printf("  -binary                Ustawia format zapisu wynikow w pliku na format binarny\n");
    printf("  -verbose               Umozliwia podglad procesu dzialania kodu, wyswietla dodatkowe informacje bledow\n");
    printf("  -graph <numer grafu>   Pozwala wybrac, ktory graf bedzie ciety\n");
}

int* createAssignmentArray(int numVert) {
    int* array = (int*)malloc(sizeof(int) * numVert);
    if (!array) handleError(88841, "Allocation failed in createAssignmentArray");
    for (int i = 0; i < numVert; i++) array[i] = -1;
    return array;
}

int checkBalance(Graph* graph, int* assignment, int parts, double margin) {
    int* groupCounts = (int*)calloc(parts, sizeof(int));
    if (!groupCounts) handleError(88851, "Allocation failed in check Balance");

    for (int i = 0; i < graph->numVert; i++) {
        int group = assignment[i];
        if (group < 0 || group >= parts) {
            free(groupCounts);
            return 0;
        }
        groupCounts[group]++;
    }

    int avg = graph->numVert / parts;
    int maxAllowed = (int)ceil(avg * (1.0 + margin / 100.0));
    int minAllowed = (int)floor(avg * (1.0 - margin / 100.0));

    int balanced = 1;
    for (int i = 0; i < parts; i++) {
        printf("Group %d size: %d (allowed: [%d, %d])\n", i, groupCounts[i], minAllowed, maxAllowed);
        if (groupCounts[i] < minAllowed || groupCounts[i] > maxAllowed) {
            balanced = 0;
            break;
        }
    }

    free(groupCounts);
    return balanced;
}

void printAssignmentSummary(Graph* graph, int* assignment, int parts) {
    int* groupCounts = (int*)calloc(parts, sizeof(int));
    if (!groupCounts) handleError(88861, "Allocation failed in printAssignmentSummary");

    for (int i = 0; i < parts; i++) groupCounts[assignment[i]]++;

    fprintf(stderr, "Group summary:\n");
    for (int i = 0; i < parts; i++) fprintf(stderr, "Group %d: %d vertices\n", i, groupCounts[i]);

    free(groupCounts);
}


int countCutEdges(Graph* graph, int* assignment, int parts) {
    int cutEdges = 0;
    for (int i = 0; i < graph->numVert; i++) {
        int group = assignment[i];
        Node* neighbor = graph->vertexData[i].neighborsHead;
        while (neighbor != NULL) {
            int neighborGroup = assignment[neighbor->vertex];
            if (group != neighborGroup) cutEdges++;
            neighbor = neighbor->next;
        }
    }
    return cutEdges / 2; //every edge is counted twice
}