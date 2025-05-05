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
    int* rowPointersOut = NULL;
    int* connectionNodes = NULL;
    int* edgeGroupPointersOut = NULL;

    // Write line 1
    if (fprintf(file, "%d\n", graph->maxDim) < 0)
    {
        fprintf(stderr, "Error: Failed to write Line 1\n");
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut); // Changed label
    }
    printf("Info: Line 1 written\n");

    // Write line 2 and 3
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
    rowPointersOut = calloc(numRows + 1, sizeof(int));
    if (!rowPointersOut)
    {
        fprintf(stderr, "Error: Failed to allocate memory for output row pointers\n");
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
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
        rowPointersOut[r + 1] = nodesWrittenCount;
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
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
    }
    printf("Info: Line 2 written %d\n", nodesWrittenCount);

    if (nodesWrittenCount != graph->numVert)
    {
        fprintf(stderr, "Error: Mismatch writing Line 2, wrote: %d nodes, should be: %d\n", nodesWrittenCount, graph->numVert);
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
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
        if (fprintf(file, "%d", rowPointersOut[i]) < 0)
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
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
    }
    printf("Info: Line 3 written %d\n", numRows + 1);


    // Line 4 and 5
    printf("Info: Creating line 4 and 5\n");
    int connNodesCapacity = graph->numVert > 0 ? graph->numVert * 2 : 10; //todo clearer names
    connectionNodes = malloc(connNodesCapacity * sizeof(int));

    int groupPtrsCapacity = graph->numVert > 0 ? graph->numVert + 1 : 10;
    edgeGroupPointersOut = malloc(groupPtrsCapacity * sizeof(int));

    if (!connectionNodes || !edgeGroupPointersOut)
    {
        fprintf(stderr, "Error: Failed to allocate memory for line 4 and 5\n");
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
    }

    int connNodeIdx = 0;
    int groupPtrIdx = 0;

    edgeGroupPointersOut[groupPtrIdx++] = 0;

    for (int hubId = 0; hubId < graph->numVert; ++hubId)
    {
        Node* neighbor = graph->vertexData[hubId].neighborsHead;

        int createsGroup = 0;
        while(neighbor != NULL)
         {
            // Write a group only if neighbour Id is higher to avoid double edges, np. 1->3 and 3->1
            if (neighbor->vertex > hubId)
            {
                createsGroup = 1;
                break;
            }
            neighbor = neighbor->next;
        }

        if (createsGroup)
        {
            // Add main node
            if (connNodeIdx >= connNodesCapacity)
            {
                int newCapacity = connNodesCapacity * 2;
                if (newCapacity <= connNodesCapacity) newCapacity = connNodesCapacity + 1024;

                int* temp = realloc(connectionNodes, newCapacity * sizeof(int));
                if (!temp)
                {
                    fprintf(stderr, "Error: Failed to realloc connectionNodes array\n");
                    return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
                }
                connectionNodes = temp;
                connNodesCapacity = newCapacity;
            }
            connectionNodes[connNodeIdx++] = hubId;

            // Add rest nodes with Id > main
            neighbor = graph->vertexData[hubId].neighborsHead;
            while(neighbor != NULL)
            {
                if (neighbor->vertex > hubId)
                {
                    if (connNodeIdx >= connNodesCapacity)
                    {
                        int newCapacity = connNodesCapacity * 2;
                        if (newCapacity <= connNodesCapacity) newCapacity = connNodesCapacity + 1024;

                        int* temp = realloc(connectionNodes, newCapacity * sizeof(int));
                        if (!temp)
                        {
                            fprintf(stderr,"Error: Failed to realloc connectionNodes array\n");
                            return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
                        }

                        connectionNodes = temp;
                        connNodesCapacity = newCapacity;
                    }
                    connectionNodes[connNodeIdx++] = neighbor->vertex;
                }
                neighbor = neighbor->next;
            }

            if (groupPtrIdx >= groupPtrsCapacity)
            {
                int newCapacity = groupPtrsCapacity * 2;
                if (newCapacity <= groupPtrsCapacity) newCapacity = groupPtrsCapacity + 1024;

                int* temp = realloc(edgeGroupPointersOut, newCapacity * sizeof(int));
                if (!temp)
                {
                    fprintf(stderr, "Error: Failed to realloc edgeGroupPointersOut array\n");
                    return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
                }
                edgeGroupPointersOut = temp;
                groupPtrsCapacity = newCapacity;
            }
            edgeGroupPointersOut[groupPtrIdx++] = connNodeIdx;
        }
    }

    // Write line 4
    for (int i = 0; i < connNodeIdx; ++i)
    {
        if (i > 0)
        {
            if (fprintf(file, ";") < 0)
            {
                errorFlag = -1;
                break;
            }
        }
        if (fprintf(file, "%d", connectionNodes[i]) < 0)
        {
            errorFlag = -1;
            break;
        }
    }

    if (errorFlag == 0 && connNodeIdx > 0)
    {
        if (fprintf(file, "\n") < 0)
        {
            errorFlag = -1;
        }
    }
    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 4\n");
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
    }
    printf("Info: Line 4 written %d\n", connNodeIdx);

    // Write line 5
    for (int i = 0; i < groupPtrIdx - 1; ++i)
    {
        if (i > 0)
        {
            if (fprintf(file, ";") < 0)
            {
                errorFlag = -1;
                break;
            }
        }
        if (fprintf(file, "%d", edgeGroupPointersOut[i]) < 0)
        {
            errorFlag = -1;
            break;
        }
    }
    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 5\n");
        return cleanupError(file, rowPointersOut, connectionNodes, edgeGroupPointersOut);
    }
    printf("Info: Line 5 written %d\n", groupPtrIdx);

    free(rowPointersOut);
    free(connectionNodes);
    free(edgeGroupPointersOut);

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error: Failed to close output file after successful write");
        return -1; // Error while closing
    }

    printf("Info: Finished graph save successfully.\n");

    return 0;
}

int cleanupError(FILE *file, int* rowPointers, int* connNodes, int* groupPtrs)
{
    fprintf(stderr, "Error: There was a mishap during writing to a file, cleaning up\n");

    free(rowPointers);
    free(connNodes);
    free(groupPtrs);

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
