#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

void handleError(int errorCode, const char *message) {
    fprintf(stderr, "Kod bledu %d: %s\n", errorCode, message);
    exit(EXIT_FAILURE);
}

double measureTime() {
    clock_t start = clock();
    // Do dokonczenia jak bedziemy mieli napisane algorytmy
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

void printHelp() {
    printf("Uzycie: ./Cut [ilosc grup do podzialu] [margines procentowy opisujacy roznice ilosci wezlow] - i [nazwa pliku z grafem]\n");
    printf("Parametry:\n");
    printf("  -help                  Wyswietla instrukcje wywolania programu oraz jego argumenty wywolania\n");
    printf("  -input <nazwa pliku>   Okresla plik wejsciowy, z ktorego program ma wczytac dane\n");
    printf("  -output <nazwa pliku>  Okresla plik wyjsciowy, do ktorego program zapisze wyniki\n");
    printf("  -binary                Ustawia format zapisu wynikow w pliku na format binarny\n");
    printf("  -verbose               Umozliwia podglad procesu dzialania kodu, wyswietla dodatkowe informacje bledow\n");
    printf("  -graph <numer grafu>   Pozwala wybrac, ktory graf bedzie ciety\n");
}