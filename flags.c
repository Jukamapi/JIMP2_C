#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "utils.h"

ProgramFlags handleFlags(int argc, char *argv[]) {
    ProgramFlags flags = {
        .inputFile = NULL,
        .outputFile = NULL,
        .binaryFormat = 0,
        .force = 0,
        .verbose = 0,
        .count = 0,
        .measure = 0,
        .graphIndex = 0,
        .parts = 2,
        .margin = 0.0,
        .algorithm = DIJKSTRA_BASED,
        .valid = 1
    };

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-help")) {
            printHelp();
            flags.valid = 0;
            return flags;

        } else if (!strcmp(argv[i], "-input") && i + 1 < argc) {
            flags.inputFile = argv[++i];

        } else if (!strcmp(argv[i], "-output") && i + 1 < argc) {
            flags.outputFile = argv[++i];

        } else if (!strcmp(argv[i], "-binary")) {
            flags.binaryFormat = 1;

        } else if (!strcmp(argv[i], "-force")) {
            flags.force = 1;

        } else if (!strcmp(argv[i], "-verbose")) {
            flags.verbose = 1;

        } else if (!strcmp(argv[i], "-count")) {
            flags.count = 1;

        } else if (!strcmp(argv[i], "-time")) {
            flags.measure = 1;

        } else if (!strcmp(argv[i], "-graph") && i + 1 < argc) {
            flags.graphIndex = atoi(argv[++i]);

        } else if (!strcmp(argv[i], "-algorithm") && i + 1 < argc) {
            i++;
            char *alg = argv[i];
            for (int j = 0; alg[j]; j++) alg[j] = toupper((unsigned char)alg[j]);

            if (!strcmp(alg, "DIJKSTRA")) flags.algorithm = DIJKSTRA_BASED;
            else if (!strcmp(alg, "KERNIGHAN_LIN")) flags.algorithm = KERNIGHAN_LIN;
            else handleError(2, "Unknown algorithm given in -algorithm");

        } else if (flags.parts == 2 && sscanf(argv[i], "%d", &flags.parts) == 1) {
            // first value without flag - number of parts
        } else if (flags.margin == 0.0 && sscanf(argv[i], "%lf", &flags.margin) == 1) {
            // second value without flag - margin
        } else {
            handleError(3, "Unknown or incomplete flag.");
        }
    }

    if (!flags.inputFile || !flags.outputFile) {
        handleError(4, "-input or -output missing");
    }

    return flags;
}
