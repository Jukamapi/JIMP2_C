#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "readFile.h"
#include "writeFile.h"
#include "utils.h"


// Config
int numPartitions = 0;       // Mandatory arg 1
double margin = 0.0;         // Mandatory arg 2
char* inputFile = NULL;      // From --input
char* outputFileBase = NULL; // Base name from --output
bool verboseMode = false;    // From --verbose
int targetGraphIndex = 1;    // From --graph (default to 1)
bool showHelp = false;       // From --help
int binary = 0;
bool outputBaseWasDuplicated = false;


int main(int argc, char *argv[])
{

    int opt;
    static struct option long_options[] =
    {
        {"help", no_argument, 0, 'h'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"verbose", no_argument, 0, 'v'},
        {"graph", required_argument, 0, 'g'},
        {"binary", no_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    int long_index = 0;

    while ((opt = getopt_long(argc, argv, "hi:o:vg:", long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case 'b':
                binary = 1;
                break;
            case 'h':
                showHelp = true;
                break;
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFileBase = optarg;
                break;
            case 'v':
                verboseMode = true;
                break;
            case 'g':
                char *endptr;
                long val = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || endptr == optarg || val <= 0 || val > INT_MAX)
                {
                    fprintf(stderr, "Error: Invalid graph index '%s'. Must be a positive integer.\n", optarg);
                    return 1;
                }
                targetGraphIndex = (int)val;
                break;
            case '?':
                fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (showHelp)
    {
        printHelp();
        return 0;
    }

    if (optind + 2 > argc)
    {
        fprintf(stderr, "Error: Missing arguments <numPartitions> and <margin>\n");
        return 1;
    }


    char* endptrP;
    long valP = strtol(argv[optind], &endptrP, 10);
    if (*endptrP != '\0' || endptrP == argv[optind] || valP <= 1 || valP > INT_MAX)
    {
        fprintf(stderr, "Error: Invalid <numPartitions> '%s'. Must be an integer > 1.\n", argv[optind]);
        return 1;
    }
    numPartitions = (int)valP;
    optind++;

    char* endptrM;
    margin = strtod(argv[optind], &endptrM);
    if (*endptrM != '\0' || endptrM == argv[optind] || margin < 0.0)
    {
        fprintf(stderr, "Error: Invalid <margin> '%s'. Must be a non-negative number.\n", argv[optind]);
        return 1;
    }

   if (inputFile == NULL)
   {
        fprintf(stderr, "Input nust be specified using --input or -i\n");
        return 1;
   }

   if (outputFileBase == NULL)
   {
        size_t len = strlen(inputFile);
        outputFileBase = malloc(len + 1); // Allocate memory
        if (!outputFileBase)
        {
            fprintf(stderr, "Error: Failed while allocating memory for default output base");
            return 1;
        }
        strcpy(outputFileBase, inputFile);
        outputBaseWasDuplicated = true; // Mark that we allocated memory

        char *dot = strrchr(outputFileBase, '.');
        char *slash = strrchr(outputFileBase, '/'); // Or '\\' on Windows but Cut dont support

        if (dot != NULL && (slash == NULL || dot > slash))
        {
            *dot = '\0';
        }
        printf("Info: No output base specified, using default: '%s'\n", outputFileBase);
    }


    printf("Info: Attempting to load graph %d from: %s\n", targetGraphIndex, inputFile);
    Graph *myGraph = loadGraph(inputFile, targetGraphIndex);

    if (myGraph)
    {
        printf("Info: Successfully loaded graph %d from %s\n", targetGraphIndex, inputFile);
        printGraph(myGraph);

        // Algorithms here
        // dla W
        // np partitionGraph(...)

        // Writing to a file
        printf("Info: Attempting to save graph %d\n", targetGraphIndex);

        char outputFilename[FILENAME_MAX];

        //for now 1 only
        snprintf(outputFilename, sizeof(outputFilename), "%s_%d.csrrg", outputFileBase, 0);
        int saveSuccess = saveGraph(myGraph, outputFilename, binary);


        // DLA WOJTKA PODSTAWA NA POTEM
        // for (int i = 0; i < numPartitions; ++i)
        // {
        //     char outputFilename[FILENAME_MAX];

        //     snprintf(outputFilename, sizeof(outputFilename), "%s_%d.csrrg", outputFileBase, i);

        //     Graph* sub = ...

        //     if (sub)
        //     {
        //         printf("Info: Saving partition %d to %s\n", i, outputFilename);

        //         if (saveGraph(sub, outputFilename, 0) != 0)
        //         {
        //             fprintf(stderr, "Error: Failed to save subraph\n");
        //             saveSuccess = false;
        //         }

        //         freeGraph(sub);
        //     }
        //     else
        //     {
        //         fprintf(stderr, "Error: Failed to save subraph\n");
        //         saveSuccess = false;
        //     }
        //     if (!saveSuccess) break;
        //     // 0 - normal, 1 - binary
        //     //todo savegraph
        // }
        // if (saveSuccess)
        // {
        //     printf("Info: Saving complete\n");
        // }
        // else
        // {
        //     fprintf(stderr, "Error: Saving encountered errors\n");
        // }

        freeGraph(myGraph);
        if (outputBaseWasDuplicated) free(outputFileBase);
        printf("Info: Graph memory freed.\n");

        if (saveSuccess != 0)
        {
            printf("Info: Application finished with errors\n");
            return 1;
        }
        else
        {
            printf("Info: Application finished with success\n");
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "Error: Failed to load graph %d\n", targetGraphIndex);
        if (outputBaseWasDuplicated) free(outputFileBase);
        return 1;
    }

    return 0;
}