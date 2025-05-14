#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "readFile.h"
#include "utils.h"


// I'm making a macro because calling failGraph(file, lineBuffer, graph, colIndices, rowPointers, edgeListIndices, edgeGroupPointers) is too long.
// I miss "using" from c++ :( -KW

#define HANDLE_FAIL() failGraph(file, lineBuffer, graph, colIndices, rowPointers, edgeListIndices, edgeGroupPointers)

int failParse(int** arr, int*** arrPtr, int** countPtr)
{
    if (*arr != NULL) free(*arr);
    **arrPtr = NULL;
    **countPtr = 0;
    return -1;
}

int parseLine(char* line, int** arrPtr, int* countPtr)
{
    int capacity = 10;
    int count = 0;

    int* arr = malloc(capacity * sizeof(int));
    if (!arr)
    {
        fprintf(stderr, "Error: Failed to allocate memory for parsing numbers\n");
        return failParse(NULL, &arrPtr, &countPtr);
    }

    char* lineCopy = line;
    char* token = strtok(lineCopy, ";\n\r");

    while (token != NULL)
    {
        // Skip empty tokens
        if (strlen(token) == 0)
        {
            token = strtok(NULL, ";\n\r");
            continue;
        }

        // Increase capacity if not enough
        if (count >= capacity)
        {
            capacity *= 2;

            int* temp = realloc(arr, capacity * sizeof(int));
            if (!temp)
            {
                fprintf(stderr, "Error: Failed to allocate memory for parsing numbers\n");
                return failParse(&arr, &arrPtr, &countPtr);
            }

            arr = temp;
        }

        char* endPtr;
        long val = strtol(token, &endPtr, 10);

        if (endPtr == token || (*endPtr != '\0' && *endPtr != '\n' && *endPtr != '\r'))
        {
            fprintf(stderr, "Error: Failed to convert token\n");
            return failParse(&arr, &arrPtr, &countPtr);
        }
        // Check if possible to cast to int
        if (val > INT_MAX || val < 0)
        {
            fprintf(stderr, "Error: Integer '%s' is out of range\n", token);
            return failParse(&arr, &arrPtr, &countPtr);
        }

        arr[count++] = (int)val;
        token = strtok(NULL, ";\n\r");
    }

    *arrPtr = arr;
    *countPtr = count;
    return 0;
}

Graph* allocateGraph(int numVertices, int maxDimension) // Reserves memory for the graph
{
    if (numVertices <= 0)
    {
        fprintf(stderr, "Error: Number of vertices must be positive\n");
        return NULL;
    }

    Graph* graph = (Graph*)malloc(sizeof(Graph));

    if (!graph)
    {
        fprintf(stderr, "Error: Failed to allocate memory for Graph\n");
        return NULL;
    }

    graph->maxDim = maxDimension;
    graph->numVert = numVertices;
    graph->vertexData = (VertexInfo*)calloc(numVertices, sizeof(VertexInfo));

    if (!graph->vertexData)
    {
        fprintf(stderr, "Error: Failed to allocate memory for adjacency list array\n");
        free(graph);
        return NULL;
    }

    return graph;
}

void addEdge(Graph* graph, int src, int dest)
{
    if (!graph || src < 0 || src >= graph->numVert)
    {
        fprintf(stderr, "Error: Invalid source vertex %d\n", src);
        return;
    }
    if (dest < 0 || dest >= graph->numVert)
    {
        fprintf(stderr, "Error: Invalid destination vertex %d\n", dest);
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error: Failed to allocate memory for new Node\n");
        return;
    }

    newNode->vertex = dest;
    newNode->next = graph->vertexData[src].neighborsHead;
    graph->vertexData[src].neighborsHead = newNode;
}


Graph* loadGraph(const char* filename, int graphIndex)
{
    if (graphIndex <= 0)
    {
        fprintf(stderr, "Error: Target graph index must be a positive integer\n");
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Failed to open file '%s'\n", filename);
        return NULL;
    }

    Graph* graph = NULL;
    int* colIndices = NULL; // Line 2
    int* rowPointers = NULL; // Line 3
    int* edgeListIndices = NULL; // Line 4
    int* edgeGroupPointers = NULL; // Line 5+


    LOG_INFO_VERBOSE("Info: Starting to load graph from file '%s' with index %d\n", filename, graphIndex);

    // Read line 1
    char* lineBuffer = NULL;
    size_t lineBufferSize = 0;

    long lineSize = getline(&lineBuffer, &lineBufferSize, file);
    if (lineSize <= 0)
    {
        fprintf(stderr, "Error: Failed to read Line 1.\n");
        return HANDLE_FAIL();
    }

    char *endptr;
    long maxDimVal = strtol(lineBuffer, &endptr, 10);

    if (endptr == lineBuffer || (*endptr != '\n' && *endptr != '\r' && *endptr != '\0') || maxDimVal <= 0 || maxDimVal > 1024)
    {
        fprintf(stderr, "Error: Invalid max dimension '%s'\n", lineBuffer);
        return HANDLE_FAIL();
    }

    int maxDim = (int)maxDimVal;
    LOG_INFO_VERBOSE("Info: Max dimensions from Line 1 = %dx%d\n", maxDim, maxDim);

    // Read line 2
    int colCount = 0;

    lineSize = getline(&lineBuffer, &lineBufferSize, file);
    if (lineSize <= 0)
    {
        fprintf(stderr, "Error: Failed to read Line 2\n");
        return HANDLE_FAIL();
    }
    if (parseLine(lineBuffer, &colIndices, &colCount) != 0)
    {
        fprintf(stderr, "Error: Failed to parse Line 2\n");
        return HANDLE_FAIL();
    }
    if (colCount <= 0)
    {
        fprintf(stderr, "Error: No nodes found\n");
        return HANDLE_FAIL();
    }

    int numVert = colCount;
    LOG_INFO_VERBOSE("Info: Determined numVert = %d\n", numVert);

    // Read line 3
    int rowPtrCount = 0;

    lineSize = getline(&lineBuffer, &lineBufferSize, file);
    if (lineSize <= 0)
    {
        fprintf(stderr, "Error: Failed to read Line 3\n");
        return HANDLE_FAIL();
    }
    if (parseLine(lineBuffer, &rowPointers, &rowPtrCount) != 0)
    {
        fprintf(stderr, "Error: Failed to parse Line 3\n");
        return HANDLE_FAIL();
    }
    LOG_INFO_VERBOSE("Info: Read %d row pointers from Line 3\n", rowPtrCount);

    if (rowPtrCount <= 0 || rowPointers[0] != 0 || rowPointers[rowPtrCount - 1] != numVert)
    {
        fprintf(stderr, "Error: Failed to validate Line 3\n");
        return HANDLE_FAIL();
    }

    LOG_INFO_VERBOSE("Info: Line 3 validated\n");

    // Read line 4
    int edgeListCount = 0;

    lineSize = getline(&lineBuffer, &lineBufferSize, file);
    if (lineSize <= 0)
    {
        fprintf(stderr, "Error: Failed to read Line 4\n");
        return HANDLE_FAIL();
    }
    if (parseLine(lineBuffer, &edgeListIndices, &edgeListCount) != 0)
    {
        fprintf(stderr, "Error: Failed to parse Line 4\n");
        return HANDLE_FAIL();
    }
    LOG_INFO_VERBOSE("Info: Read %d edge list indices from Line 4\n", edgeListCount);

    for (int i = 0; i < edgeListCount; ++i)
    {
        if (edgeListIndices[i] < 0 || edgeListIndices[i] >= numVert)
        {
            fprintf(stderr, "Error: Failed to validate Line 4\n");
            return HANDLE_FAIL();
        }
    }

    int currentGraphLine = 5;
    LOG_INFO_VERBOSE("Info: Searching for graph %d starting at line %d\n", graphIndex, currentGraphLine);

    int edgeGroupCount = 0;
    int foundGraphIndex = 0;
    while ((lineSize = getline(&lineBuffer, &lineBufferSize, file)) > 0)
    {
        foundGraphIndex++;

        if (foundGraphIndex == graphIndex)
        {
            LOG_INFO_VERBOSE("Info: Found target graph %d.\n", currentGraphLine); //todo

            if (parseLine(lineBuffer, &edgeGroupPointers, &edgeGroupCount) != 0)
            {
                fprintf(stderr, "Error: Failed to parse the graph on Line %d\n", currentGraphLine);
                return HANDLE_FAIL();
            }
            if (edgeGroupCount <= 0)
            {
                fprintf(stderr, "Error: No pointers found on line %d\n", currentGraphLine);
                return HANDLE_FAIL();
            }
            if (edgeGroupPointers[0] != 0)
            {
                fprintf(stderr, "Error: First edge group pointer on target line %d is not 0.\n", currentGraphLine);
                return HANDLE_FAIL();
            }
            if (edgeGroupPointers[edgeGroupCount - 1] > edgeListCount || edgeGroupPointers[edgeGroupCount - 1] < 0)
            {
                fprintf(stderr, "Error: Last pointer value line %d is not a valid starting index\n", currentGraphLine);
                return HANDLE_FAIL();
            }

            for (int i = 0; i < edgeGroupCount - 1; ++i)
            {
                if (edgeGroupPointers[i] > edgeGroupPointers[i+1])
                {
                    fprintf(stderr, "Error: Pointers on line %d are decreased at index %d.\n", currentGraphLine, i);
                    return HANDLE_FAIL();
                }
            }
            LOG_INFO_VERBOSE("Info: Group pointers validated.\n");

            break;
        }

        currentGraphLine++;
    }

    // Check if the graph was found
    if (foundGraphIndex < graphIndex || edgeGroupPointers == NULL)
    {
        if (feof(file))
        {
            fprintf(stderr, "Error: Reached end of file while searching. Graph index %d not found\n", graphIndex);
        }
        else if (ferror(file))
        {
            fprintf(stderr, "Error reading file while searching for graph\n");
        }
        return HANDLE_FAIL();
    }

    // Create graph
    graph = allocateGraph(numVert, maxDim); //todo
    if (!graph)
    {
        // Error messegaes are in the function
        return HANDLE_FAIL();
    }

    LOG_INFO_VERBOSE("Info: Populating vertex row/column information\n");
    int currentRow = 0;
    for (int i = 0; i < rowPtrCount - 1; ++i)
    {
        int rowStartIndex = rowPointers[i];
        int rowEndIndex = rowPointers[i+1];
        currentRow = i;
        for (int k = rowStartIndex; k < rowEndIndex; ++k)
        {
            graph->vertexData[k].vertexId = k;
            graph->vertexData[k].row = currentRow;
            graph->vertexData[k].col = colIndices[k];
        }
    }
    LOG_INFO_VERBOSE("Info: Row/column information populated\n");

    LOG_INFO_VERBOSE("Info: Building adjacency list\n");

    for (int i = 0; i < edgeGroupCount; ++i)
    {
        int groupStartIndex = edgeGroupPointers[i];
        int groupEndIndex;
        if (i == (edgeGroupCount - 1)) groupEndIndex = edgeListCount;
        else groupEndIndex = edgeGroupPointers[i + 1];

        // Check if the group has 2 nodes
        if (groupStartIndex >= groupEndIndex - 1)
        {
            LOG_INFO_VERBOSE("Info: Skipping group %d because it doesn't have enough nodes\n", i);
            continue;
        }

        // Get the main node
        int mainNode = edgeListIndices[groupStartIndex];
        if (mainNode < 0 || mainNode >= graph->numVert)
        {
            fprintf(stderr, "Error: Invalid main node index %d found in group %d\n", mainNode, i);
            return HANDLE_FAIL();
        }

        // Connect main node to every other node in the group
        for (int k = groupStartIndex + 1; k < groupEndIndex; ++k)
        {
            int restNode = edgeListIndices[k];
            if (restNode < 0 || restNode >= graph->numVert)
            {
                fprintf(stderr, "Error: Invalid rest node index %d found in group %d\n", mainNode, i);
                return HANDLE_FAIL();
            }

            // Add edges between nodes
            if (mainNode != restNode)
            {
                addEdge(graph, mainNode, restNode);
                addEdge(graph, restNode, mainNode);
            }
        }
    }
    LOG_INFO_VERBOSE("Info: Adjacency list built for graph %d.\n", graphIndex);

    fclose(file);
    free(lineBuffer);
    free(colIndices);
    free(rowPointers);
    free(edgeListIndices);
    free(edgeGroupPointers);

    LOG_INFO_VERBOSE("Info: Finished loading graph %d successfully.\n", graphIndex);
    return graph;
}

// Handle errors
Graph* failGraph(FILE* file, char* lineBuffer, Graph* graph, int* colIndices, int* rowPointers, int* edgeListIndices, int* edgeGroupPointers)
{
    fprintf(stderr, "Cleanup triggered due to error during graph loading.\n");

    if (file) fclose(file);

    free(lineBuffer);
    free(colIndices);
    free(rowPointers);
    free(edgeListIndices);
    free(edgeGroupPointers);

    if (graph) freeGraph(graph);

    return NULL;
}

// Free the graph memory
void freeGraph(Graph* graph)
{
    if (!graph) return;
    if (graph->vertexData)
    {
        for (int i = 0; i < graph->numVert; ++i)
        {
            Node* current = graph->vertexData[i].neighborsHead;
            while (current != NULL)
            {
                Node* temp = current;
                current = current->next;
                free(temp);
            }
        }
        free(graph->vertexData);
        graph->vertexData = NULL;
    }
}

// Print the graph
void printGraph(const Graph* graph)
{
    if (!graph)
    {
        printf("Graph is NULL\n");
        return;
    }

    printf("Graph:\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < graph->numVert; ++i)
    {
        printf("Node %3d (R=%d, C=%d): ", i, graph->vertexData[i].row, graph->vertexData[i].col);

        Node* current = graph->vertexData[i].neighborsHead;
        if (!current)
        {
            printf("-> NULL");
        }
        else
        {
            while (current != NULL)
            {
                printf("-> %d", current->vertex);
                current = current->next;
            }
        }
        printf("\n");
    }
     printf("----------------------------------------\n");
}