#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "writeFile.h"
#include "readFile.h"

// There are way to many nested things here but I don't really have time to fix it, sorry to whoever has to read this -KW

static uint64_t bitBuffer = 0;
static int bitsInBuffer = 0;

// These functions were supposed to be for 11 bit at the start, but i realised that the delta can be bigger than 11 bits which for some reason i thouthg that was max

// I also wanted to make the size of bits for these functions be possible to change so the file could for example have 6 bits if the deltas were small enough, I've written it but the complexity was way too high for me to give it to someone else to decode later so I settled on this atm.

// I know that since im using 16 bytes i might as well use short but this solution is alway Big Endian so I'm leaving it

int writeByte(unsigned char byte, FILE* file)
{
    if (fwrite(&byte, 1, 1, file) != 1)
    {
        fprintf(stderr, "Error: Failed to write byte to binary file\n");
        return -1;
    }
    return 0;
}

int writeBits(uint32_t value, int bitsToWrite, FILE* file)
{
    if (bitsToWrite <= 0 || bitsToWrite > 32)
    {
        fprintf(stderr, "Error: Invalid bitsToWrite %d\n", bitsToWrite);
        return -1;
    }
    if (value >= (1U << bitsToWrite) && bitsToWrite < 16)
    {
         fprintf(stderr, "Error: Value %u is too large for %d bits\n", value, bitsToWrite);
         return -1;
    }

    bitBuffer <<= bitsToWrite;
    bitBuffer |= (value & ((1U << bitsToWrite) - 1));
    bitsInBuffer += bitsToWrite;

    while (bitsInBuffer >= 8)
    {
        unsigned char byteToWrite = (unsigned char)((bitBuffer >> (bitsInBuffer - 8)) & 0xFF);
        if (writeByte(byteToWrite, file) != 0)
        {
            return -1;
        }
        bitsInBuffer -= 8;
    }
    return 0;
}

int writeSigned16Bit(int value, FILE* file)
{
    uint16_t magnitude = (uint16_t)abs(value);
    uint16_t sign_bit = (value < 0) ? 1 : 0;

    if (magnitude > 0x7FFF)
    {
        fprintf(stderr, "Error: Value %d is too large for 16 bits\n", value);
        return -1;
    }

    uint16_t encoded_value = (sign_bit << 15) | magnitude;
    return writeBits(encoded_value, 16, file);
}

int flushBuffer(FILE* file)
{
    if (bitsInBuffer > 0)
    {
        unsigned char lastByte = (unsigned char)((bitBuffer << (8 - bitsInBuffer)) & 0xFF);

        if (writeByte(lastByte, file) != 0)
        {
            return -1;
        }
        bitsInBuffer = 0;
        bitBuffer = 0;
    }
    return 0;
}

int writeArrayDataDeltas16Bit(const int* array, int count, FILE* file)
{
    if (count < 0)
    {
        fprintf(stderr, "Error: Negative count %d\n", count);
        return -1;
    }
    if (count == 0) return 0;

    // First element absolute
    if (writeSigned16Bit(array[0], file) != 0)
    {
        fprintf(stderr, "Error: Failed to write absolute 16 bit value %d\n", array[0]);
        return -1;
    }
    int prevAbsValue = array[0];

    // Rest deltas
    for (int i = 1; i < count; ++i)
    {
        int delta = array[i] - prevAbsValue;
        if (writeSigned16Bit(delta, file) != 0)
        {
            fprintf(stderr, "Error: Failed to write 16 bit delta\n");
            return -1;
        }
        prevAbsValue = array[i];
    }
    return 0;
}

int writeCountAndArrayDeltas16Bit(const int* array, int count, FILE* file)
{
    uint32_t uCount = (uint32_t)count;

    if (writeUint32BigEndian(uCount, file) != 0)
    {
        fprintf(stderr, "Error: Failed to write count (uint32_t) before delta array\n");
        return -1;
    }
    return writeArrayDataDeltas16Bit(array, count, file);
}

int writeUint32BigEndian(uint32_t value, FILE* file)
{
    unsigned char bytes[4];
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;

    if (fwrite(bytes, 1, 4, file) != 4)
    {
        fprintf(stderr, "Error: Failed while writing uint32_t in Big Endian\n");
        return -1;
    }
    return 0;
}


int saveGraph(const Graph* graph, const char* filename, int binaryFormat)
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

    const char* fileMode = binaryFormat ? "wb" : "w";
    FILE* file = fopen(filename, fileMode);
    if (!file)
    {
        fprintf(stderr, "Error: Failed to open file '%s'\n", filename);
        return -1;
    }

    printf("Info: Starting writing graph to '%s'\n", filename);

    if (binaryFormat)
    {
        bitBuffer = 0;
        bitsInBuffer = 0;
    }

    int errorFlag = 0;
    int* rowPointers = NULL;
    int* groupedNodeIndices = NULL;
    int* groupPointers = NULL;
    int* line2DataForBinary = NULL;

    // Write line 1
    if (binaryFormat)
    {
        int maxDimArray[] = {graph->maxDim > 0 ? graph->maxDim : 0};

        uint32_t countLine1 = 1;
        if (writeUint32BigEndian(countLine1, file) != 0)
        {
            fprintf(stderr, "Error: Failed while writing count for Line 1\n");
            errorFlag = -1;
        }
        else
        {
            if (writeSigned16Bit(maxDimArray[0], file) != 0) errorFlag = -1;
        }
    }
    else
    {
        if (fprintf(file, "%d\n", graph->maxDim) < 0)
        {
            fprintf(stderr, "Error: Failed to write Line 1\n");
            errorFlag = -1;
        }
    }
    if (errorFlag != 0) return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    printf("Info: Line 1 written\n");

    // Create line 2 and 3
    printf("Info: Creating lines 2 and 3\n");
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

    if (!rowPointers && (numRows + 1 >0))
    {
        fprintf(stderr, "Error: Failed to allocate memory for row pointers\n");
        errorFlag = -1;
    }

    if (binaryFormat)
    {
        line2DataForBinary = malloc(graph->numVert * sizeof(int));
        if (!line2DataForBinary)
        {
            fprintf(stderr, "Error: Failed to allocate memory for Line 2 binary data\n");
            errorFlag = -1;
        }
    }
    if (errorFlag != 0) return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);


    // Write line 2

    int nodesWrittenCount = 0;
    for (int r = 0; r < numRows; ++r)
    {
        int nodesInThisRow = 0;
        for (int i = 0; i < graph->numVert; ++i)
        {
            if (graph->vertexData[i].row == r)
            {
                if(binaryFormat)
                {
                    line2DataForBinary[nodesWrittenCount] = graph->vertexData[i].col;
                }
                else
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
                nodesWrittenCount++;
            }
        }
        if(errorFlag != 0) break;
        rowPointers[r + 1] = nodesWrittenCount;
    }

    // Write newline if no error
    if(errorFlag == 0)
    {
        if (binaryFormat)
        {
            if (writeCountAndArrayDeltas16Bit(line2DataForBinary, nodesWrittenCount, file) != 0) errorFlag = -1;
        }
        else
        {
            if (fprintf(file, "\n") < 0)
            {
                errorFlag = -1;
            }
        }
    }
    free(line2DataForBinary);
    line2DataForBinary = NULL;

    if(errorFlag !=0)
    {
        fprintf(stderr, "Error: Failed to write Line 2\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 2 written %d\n", nodesWrittenCount);

    if (nodesWrittenCount != graph->numVert)
    {
        fprintf(stderr, "Error: Mismatch writing Line 2, wrote: %d, should be: %d\n", nodesWrittenCount, graph->numVert);
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
     }

    // Write line 3
    if (binaryFormat)
    {
        if (writeCountAndArrayDeltas16Bit(rowPointers, numRows + 1, file) != 0) errorFlag = -1;
    }
    else
    {
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
    if (binaryFormat)
    {
        if (writeCountAndArrayDeltas16Bit(groupedNodeIndices, groupedNodeIndicesCount, file) != 0) errorFlag = -1;
    }
    else
    {
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
    }

    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 4\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 4 written %d\n", groupedNodeIndicesCount);

    // Write line 5
    if (binaryFormat)
    {
        if (writeCountAndArrayDeltas16Bit(groupPointers, groupPointersCount - 1, file) != 0 ) errorFlag = -1;
    }
    else
    {
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
    }

    if(errorFlag != 0)
    {
        fprintf(stderr, "Error: Failed to write Line 5\n");
        return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
    }
    printf("Info: Line 5 written %d\n", groupPointersCount - 1);

    if(binaryFormat && errorFlag == 0) //todo
    {
        if (flushBuffer(file) != 0)
        {
            return cleanupError(file, rowPointers, groupedNodeIndices, groupPointers);
        }
    }

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
    fprintf(stderr, "Error: There was a mishap during writing to a file, cleaning up.\n");

    free(rowPointers);
    free(groupedNodeIndices);
    free(groupPointers);


    if (file != NULL)
    {
        if (fclose(file) != 0)
        {
            fprintf(stderr, "Error: Failed to close output file during error cleanup\n");
        }
    }

    return -1;
}
