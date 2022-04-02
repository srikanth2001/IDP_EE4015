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
#include <string.h>    // memset, strcat, strtol
#include <zstd.h>      // presumes zstd library is installed
#include <time.h>      // clock
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t chunkSize; 
static double sumOfTimes = 0, compressionRatio;
static size_t compressedSize, headerSize;

size_t min(size_t a, size_t b){
    return (a < b) ? a : b;
}

static void compress(const char* fname, const char* oname)
{
    size_t fSize, outSize = 0;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    size_t numOfChunks = (fSize + chunkSize - 1ll) /  chunkSize; // ceil(fSize / chunkSize)
    void* const out = malloc_orDie(fSize);
    void* const header = malloc_orDie(50 * (numOfChunks + 1)); // For indexes < 10^10

    char buff[40];
    sprintf(buff, "%ld %ld ", numOfChunks, fSize);
    // printf("No. of chunks: %d\n", numOfChunks);
    memcpy((unsigned char*)header, buff, strlen(buff));
    headerSize = strlen(buff);

    clock_t begin = clock();

    for(size_t chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += chunkSize){
        size_t realSize = min(chunkSize, fSize - offset);
        // printf("%ld ", realSize);
        
        size_t const cBuffSize = ZSTD_compressBound(realSize);
        void* const cBuff = malloc_orDie(cBuffSize);
        size_t const cSize = ZSTD_compress(cBuff, cBuffSize, (unsigned char*)fBuff + offset, realSize, 1);
        CHECK_ZSTD(cSize);

        memcpy((unsigned char*)out + outSize, cBuff, cSize);

        sprintf(buff, "%ld ", outSize); // header entries for each chunk
        memcpy((unsigned char*)header + headerSize, buff, strlen(buff));
        
        headerSize += strlen(buff);
        outSize += cSize;

        free(cBuff);
    }

    clock_t end = clock();
    double timeSpent = (end - begin) / (double)CLOCKS_PER_SEC;
    sumOfTimes += timeSpent;
    compressedSize = outSize + headerSize;
    compressionRatio = fSize / (double)compressedSize;

    free(fBuff);
    
    saveFile_orDie(oname, header, headerSize, "wb");
    saveFile_orDie(oname, out, outSize, "ab");
    free(out);
    free(header);
    /* success */
    // printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)(headerSize + outSize), oname);
}
 
 
static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 30;
    void* outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat((char*)outSpace, filename);
    strcat((char*)outSpace, ".zst");
    return (char*)outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
    
    // Comment this check condition if you are running the benchmarking for multiple dictionaries
    if (argc < 3) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }

    char* ptr;
    // The below code is for compressing using the chunk size specified in argv
    chunkSize = (size_t)strtol(argv[argc - 1], &ptr, 10);

    const int noi = 5;
    
    for(int iter = 0; iter < noi; iter++){
        for (int u=1; u<argc-1; u++) {
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            compress(inFilename, outFilename);       
            free(outFilename);
        }
    }
    printf("Total compressed file size: %ld B\n", compressedSize);
    printf("Header size: %ld B\n", headerSize);
    printf("Compression ratio: %lf\n", compressionRatio);
    printf("Avg. compression time: %lf s\n", sumOfTimes / noi);

    return 0;
}
