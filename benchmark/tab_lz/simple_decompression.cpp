/*
 * Copyright (c) Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */
 
 
#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // strtok, strtol
#include <zstd.h>      // presumes zstd library is installed
#include <time.h>      // clock
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t numOfChunks = 0, headerSize = 0, fSize = 0;
static double sumRATime = 0, sumTotalTime = 0, randomAccessTime = 0, totalTime = 0; 

size_t* readHeader(void* const cBuff, size_t cSize){
    char delim[2] = " ";
    char* token = strtok((char*)cBuff, delim);
    headerSize += strlen(token) + 1;
    char* ptr;
    numOfChunks = strtol(token, &ptr, 10);

    token = strtok(NULL, delim);
    fSize = strtol(token, &ptr, 10);
    headerSize += strlen(token) + 1;

    size_t* table = (size_t*)malloc_orDie(sizeof(size_t) * (numOfChunks + 1));

    for(size_t cnt = 0; cnt < numOfChunks; cnt++){
        token = strtok(NULL, delim);
        table[cnt] = strtol(token, &ptr, 10);
        headerSize += strlen(token) + 1;
    }
    return table;
}
 
static void decompress(const char* fname, const char* oname)
{
    size_t cSize;
    void* const cBuff = mallocAndLoadFile_orDie(fname, &cSize);
    
    size_t* table = readHeader(cBuff, cSize);

    // printf("No. of chunks: %d\n", numOfChunks);
    size_t offset = headerSize;
    unsigned long long outSize = 0; 
    table[numOfChunks] = cSize - offset;

    void* const out = malloc_orDie(fSize);
    
    randomAccessTime = 0, totalTime = 0;
    clock_t begin = clock();

    for(size_t chunk = 0, pos = offset + table[0]; chunk < numOfChunks; chunk++){
        clock_t cBegin = clock();
        size_t cBlockSize = table[chunk + 1] - table[chunk];
        unsigned long long const rSize = ZSTD_getFrameContentSize((unsigned char*)cBuff + pos, cBlockSize);
        CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
        CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
        void* const rBuff = malloc_orDie((size_t)rSize); // To store the decompressed data

        size_t const dSize = ZSTD_decompress(rBuff, rSize, (unsigned char*)cBuff + pos, cBlockSize);
        CHECK_ZSTD(dSize);
        /* When zstd knows the content size, it will error if it doesn't match. */
        CHECK(dSize == rSize, "Impossible because zstd will check this condition!");
        clock_t cEnd = clock();
        randomAccessTime += (double)(cEnd - cBegin);
        
        memcpy((unsigned char*)out + outSize, rBuff, rSize); // Copying rBuff to out
        pos += cBlockSize;
        outSize += rSize;
        free(rBuff);
    }

    clock_t end = clock();
    totalTime = (end - begin) / (double)CLOCKS_PER_SEC;    
    randomAccessTime /= ((double)numOfChunks * CLOCKS_PER_SEC);
    sumRATime += randomAccessTime;
    sumTotalTime += totalTime;

    saveFile_orDie(oname, out, outSize, "wb");

    free(out);
    free(cBuff);
    free(table);
    /* success */
    // printf("%25s : %6u -> %25s : %7u \n", fname, (unsigned)cSize, oname, (unsigned)outSize);
}

static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL;
    char* outSpace = (char*)malloc_orDie(sizeof(char) * outL);
    memset(outSpace, 0, sizeof(char) * outL);
    for(int i = 0; i < inL - 4; i++) // remove the .zst part
        outSpace[i] = filename[i];
    return outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
 
    if (argc<2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    const int noi = 10;
    
    for(int iter = 0; iter < noi; iter++){
        for (int u=1; u<argc; u++){
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            numOfChunks = 0, headerSize = 0;
            decompress(inFilename, outFilename);
            free(outFilename);
        }
    }
    printf("Avg. random access time for a chunk: %lf s\n", sumRATime / noi);
    printf("Total time for decompression: %lf s\n\n", sumTotalTime / noi);

    // printf("All %u files correctly decoded\n", argc-2);
    return 0;
}