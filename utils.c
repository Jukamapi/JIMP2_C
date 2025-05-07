#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

void handleError(int errorCode, const char *message) {
    fprintf(stderr, "Error code %d: %s\n", errorCode, message);
    exit(EXIT_FAILURE);
}

double measureTime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

void printHelp() {
    printf("Uzycie: ./MinCut [ilosc grup do podzialu] [margines procentowy opisujacy roznice ilosci wezlow] <nazwa pliku z grafem>\n");
    printf("Parametry:\n");
    printf("  -help                  Wyswietla instrukcje wywolania programu oraz jego argumenty wywolania\n");
    printf("  -input <nazwa pliku>   Okresla plik wejsciowy, z ktorego program ma wczytac dane\n");
    printf("  -output <nazwa pliku>  Okresla plik wyjsciowy, do ktorego program zapisze wyniki\n");
    printf("  -force                 Wymusza dzielenie grafu przy danych wejsciowych wychodzacych poza zakres\n");
    printf("  -binary                Ustawia format zapisu wynikow w pliku na format binarny\n");
    printf("  -verbose               Umozliwia podglad procesu dzialania kodu, wyswietla dodatkowe informacje bledow\n");
    printf("  -count                 Wypisuje ilosc przerwanych krawedzi\n");
    printf("  -time                  Mierzy czas dzielenia grafow\n");
    printf("  -algorithm <algorytm>  Ustawia algorytm ciecia grafow\n");
    printf("  -graph <numer grafu>   Pozwala wybrac, ktory graf bedzie ciety\n");
}