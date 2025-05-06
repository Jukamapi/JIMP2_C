#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "writeFile.h"
#include "readFile.h"

int saveGraph(const Graph* graph, const char* filename)
{
    if (!graph || !graph->vertexData)
    {
        fprintf(stderr, "Error: Cannot save NULL or invalid graph\n");
        return -1;
    }
    if (graph->numVert <= 0)
    {
        printf("Info: Skipping an empty file\n");
        return 0;
    }

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: Failed to open file '%s'\n", filename);
        return -1;
    }

    printf("Info: Starting writing graph to '%s'\n", filename);
    int errorFlag = 0;
    int* rowPointers = NULL;
    int* groupedNodeIndices = NULL;
    int* groupPointers = NULL;

    // Write line 1
    if (fprintf(file, "%d\n", graph->maxDim) < 0)
    {
        fprintf(stderr, "Error: Failed to write Line 1\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers); // Changed label
    }
    printf("Info: Line 1 written\n");

    // Create line 2 and 3
    printf("Info: Writing lines 2 and 3\n");
    int maxRowInUse = -1;
    for (int i = 0; i < graph->numVert; ++i)
    {
        if (graph->vertexData[i].row > maxRowInUse)
        {
            maxRowInUse = graph->vertexData[i].row;
        }
    }

    int numRows = maxRowInUse + 1;
    rowPointers = calloc(numRows + 1, sizeof(int));
    if (!rowPointers)
    {
        fprintf(stderr, "Error: Failed to allocate memory for output row pointers\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }

    // Write line 2
    int nodesWrittenCount = 0;
    for (int r = 0; r < numRows; ++r)
    {
        int nodesInThisRow = 0;
        for (int i = 0; i < graph->numVert; ++i)
        {
            if (graph->vertexData[i].row == r)
            {
                if (nodesWrittenCount > 0 || nodesInThisRow > 0)
                {
                    if (fprintf(file, ";") < 0)
                    {
                        errorFlag = -1;
                        break;
                    }
                }
                if (fprintf(file, "%d", graph->vertexData[i].col) < 0)
                {
                    errorFlag = -1;
                    break;
                }
                nodesInThisRow++;
            }
        }
        if(errorFlag != 0) break;
        nodesWrittenCount += nodesInThisRow;
        rowPointers[r + 1] = nodesWrittenCount;
    }

    // Write newline if no error
    if(errorFlag == 0)
    {
        if (fprintf(file, "\n") < 0)
        {
            errorFlag = -1;
        }
    }

    if(errorFlag !=0)
    {
        fprintf(stderr, "Error: Failed to write Line 2\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 2 written %d\n", nodesWrittenCount);

    if (nodesWrittenCount != graph->numVert)
    {
        fprintf(stderr, "Error: Mismatch writing Line 2, wrote: %d nodes, should be: %d\n", nodesWrittenCount, graph->numVert);
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
     }

    // Write line 3
    for (int i = 0; i <= numRows; ++i)
    {
        if (i > 0)
        {
            if (fprintf(file, ";") < 0)
            {
                errorFlag = -1;
                break;
            }
        }
        if (fprintf(file, "%d", rowPointers[i]) < 0)
        {
            errorFlag = -1;
            break;
        }
    }
    if(errorFlag == 0)
    {
        if (fprintf(file, "\n") < 0)
        {
            errorFlag = -1;
        }
    }
    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed while writing Line 3\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 3 written %d\n", numRows + 1);


    // Line 4 and 5
    printf("Info: Creating line 4 and 5\n");
    int groupedNodeIndicesCapacity = graph->numVert > 0 ? graph->numVert * 2 : 10; //todo clearer names
    groupedNodeIndices = malloc(groupedNodeIndicesCapacity * sizeof(int));

    int groupPointersCapacity = graph->numVert > 0 ? graph->numVert + 1 : 10;
    groupPointers = malloc(groupPointersCapacity * sizeof(int));

    if (!groupedNodeIndices || !groupPointers)
    {
        fprintf(stderr, "Error: Failed to allocate memory for line 4 and 5\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }

    int groupedNodeIndicesCount = 0;
    int groupPointersCount = 0;

    groupPointers[groupPointersCount++] = 0;

    for (int mainId = 0; mainId < graph->numVert; ++mainId)
    {
        Node* neighbor = graph->vertexData[mainId].neighborsHead;

        int createsGroup = 0;
        while(neighbor != NULL)
         {
            // Write a group only if neighbour Id is higher to avoid double edges, np. 1->3 and 3->1
            if (neighbor->vertex > mainId)
            {
                createsGroup = 1;
                break;
            }
            neighbor = neighbor->next;
        }

        if (createsGroup)
        {
            // Add main node
            if (groupedNodeIndicesCount >= groupedNodeIndicesCapacity)
            {
                int newCapacity = groupedNodeIndicesCapacity * 2;
                if (newCapacity <= groupedNodeIndicesCapacity) newCapacity = groupedNodeIndicesCapacity + 1024;

                int* temp = realloc(groupedNodeIndices, newCapacity * sizeof(int));
                if (!temp)
                {
                    fprintf(stderr, "Error: Failed to realloc groupedNodeIndices array\n");
                    return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
                }
                groupedNodeIndices = temp;
                groupedNodeIndicesCapacity = newCapacity;
            }
            groupedNodeIndices[groupedNodeIndicesCount++] = mainId;

            // Add rest of nodes with Id > main
            neighbor = graph->vertexData[mainId].neighborsHead;
            while(neighbor != NULL)
            {
                if (neighbor->vertex > mainId)
                {
                    if (groupedNodeIndicesCount >= groupedNodeIndicesCapacity)
                    {
                        int newCapacity = groupedNodeIndicesCapacity * 2;
                        if (newCapacity <= groupedNodeIndicesCapacity) newCapacity = groupedNodeIndicesCapacity + 1024;

                        int* temp = realloc(groupedNodeIndices, newCapacity * sizeof(int));
                        if (!temp)
                        {
                            fprintf(stderr,"Error: Failed to realloc groupedNodeIndices array\n");
                            return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
                        }

                        groupedNodeIndices = temp;
                        groupedNodeIndicesCapacity = newCapacity;
                    }
                    groupedNodeIndices[groupedNodeIndicesCount++] = neighbor->vertex;
                }
                neighbor = neighbor->next;
            }

            if (groupPointersCount >= groupPointersCapacity)
            {
                int newCapacity = groupPointersCapacity * 2;
                if (newCapacity <= groupPointersCapacity) newCapacity = groupPointersCapacity + 1024;

                int* temp = realloc(groupPointers, newCapacity * sizeof(int));
                if (!temp)
                {
                    fprintf(stderr, "Error: Failed to realloc groupPointers array\n");
                    return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
                }
                groupPointers = temp;
                groupPointersCapacity = newCapacity;
            }
            groupPointers[groupPointersCount++] = groupedNodeIndicesCount;
        }
    }

    // Write line 4
    for (int i = 0; i < groupedNodeIndicesCount; ++i)
    {
        if (i > 0)
        {
            if (fprintf(file, ";") < 0)
            {
                errorFlag = -1;
                break;
            }
        }
        if (fprintf(file, "%d", groupedNodeIndices[i]) < 0)
        {
            errorFlag = -1;
            break;
        }
    }

    if (errorFlag == 0 && groupedNodeIndicesCount > 0)
    {
        if (fprintf(file, "\n") < 0)
        {
            errorFlag = -1;
        }
    }
    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 4\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 4 written %d\n", groupedNodeIndicesCount);

    // Write line 5
    for (int i = 0; i < groupPointersCount - 1; ++i)
    {
        if (i > 0)
        {
            if (fprintf(file, ";") < 0)
            {
                errorFlag = -1;
                break;
            }
        }
        if (fprintf(file, "%d", groupPointers[i]) < 0)
        {
            errorFlag = -1;
            break;
        }
    }
    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 5\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 5 written %d\n", groupPointersCount);

    free(rowPointers);
    free(groupedNodeIndices);
    free(groupPointers);

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: Failed to close output file after successful write");
        return -1; // error while closing
    }

    printf("Info: Finished graph save successfully.\n");

    return 0;
}

int cleanupError(FILE *file, int* rowPointers, int* groupedNodeIndices, int* groupPointers)
{
    fprintf(stderr, "Error: There was a mishap during writing to a file, cleaning up\n");

    free(rowPointers);
    free(groupedNodeIndices);
    free(groupPointers);

    if (file != NULL && fclose(file) != 0)
    {
        fprintf(stderr, "Error: Failed to clode output file during error cleanup\n");
    }
    else if (file == NULL)
    {
        fprintf(stderr, "Error: File pointer was NULL during cleanup\n");
    }

    return -1;
}
