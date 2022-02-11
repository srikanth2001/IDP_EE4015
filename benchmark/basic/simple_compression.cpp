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
#include <string.h>    // strlen, strcat, memset
#include <zstd.h>      // presumes zstd library is installed
#include <time.h>      // clock
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static FILE* fptr;
static double sumOfTimes = 0;

void recordTime(double);
 
static void compress_orDie(const char* fname, const char* oname)
{
    size_t fSize;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    size_t const cBuffSize = ZSTD_compressBound(fSize);
    void* const cBuff = malloc_orDie(cBuffSize);
 
    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     * See the multiple_simple_compression.c example.
     */

    clock_t begin = clock();

    size_t const cSize = ZSTD_compress(cBuff, cBuffSize, fBuff, fSize, 1);
    CHECK_ZSTD(cSize);
    
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    sumOfTimes += time_spent;
    fprintf(fptr, "%lf s\n", time_spent);

    printf("%ld ", strlen((char*)cBuff));
 
    saveFile_orDie(oname, cBuff, cSize, "wb");
 
    /* success */
    printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)cSize, oname);
 
    free(fBuff);
    free(cBuff);
}
 
static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 5;
    void* const outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat(outSpace, filename);
    strcat(outSpace, ".zst");
    return (char*)outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];

    fptr = fopen("../data/simple_compression.txt", "a"); // the runtimes are stored in this file
    fprintf(fptr, "%s", "\nStarting new session for simple_compression\n");

    if (argc!=2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s FILE\n", exeName);
        return 1;
    }
    
    const char* const inFilename = argv[1];
 
    char* const outFilename = createOutFilename_orDie(inFilename);
    int numOfIterations = 10;
    for(int iter = 0; iter < numOfIterations; iter++){
        printf("Iteration-%d: ", iter + 1);
        compress_orDie(inFilename, outFilename);
    }
    fprintf(fptr, "Average runtime: %lf s\n", sumOfTimes / numOfIterations);
    free(outFilename);
    fclose(fptr);
    return 0;
}
