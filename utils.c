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
    printf("Uzycie: ./cut <liczba_czesci> <margines_%%> --input <plik.csrrg> [opcje]\n\n");
    printf("Argumenty wymagane:\n");
    printf("    <liczba_czesci>     Liczba grup, na ktore nalezy podzielic graf (np. 3)\n");
    printf("    <margines_%%>        Dopuszczalna roznica procentowa liczby wierzcholkow miedzy grupami\n");
    printf("    --input <plik>      Plik wejsciowy w formacie .csrrg\n\n");
    printf("Opcje dodatkowe:\n");
    printf("  --output <nazwa pliku>  Bazowa nazwa plikow wynikowych (domyslnie: nazwa wejsciowa bez rozszerzenia)\n");
    printf("                          Wygenerowane beda pliki: <bazowa_nazwa>_0.csrrg, <bazowa_nazwa>_1.csrrg, ...\n");
    printf("  --binary                Zapis wynikow w formacie binarnym (csrrgbin)\n");
    printf("  --verbose               Wlacza szczegolowe logowanie do stderr\n");
    printf("  --graph <numer grafu>   Wybiera, ktory graf z pliku ma byc uzyty (domyslnie: 1)\n");
    printf("  --help                  Wyswietla te pomoc\n");
}

int* createAssignmentArray(int numVert) {
    int* array = (int*)malloc(sizeof(int) * numVert);
    if (!array) handleError(41, "Allocation failed in createAssignmentArray");
    for (int i = 0; i < numVert; i++) array[i] = -1;
    return array;
}

int checkBalance(Graph* graph, int* assignment, int parts, double margin) {
    int* groupCounts = (int*)calloc(parts, sizeof(int));
    if (!groupCounts) handleError(42, "Allocation failed in check Balance");

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

int countCutEdges(Graph* graph, int* assignment) {
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