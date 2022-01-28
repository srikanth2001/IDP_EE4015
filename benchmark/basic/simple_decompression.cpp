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
#include <zstd.h>      // presumes zstd library is installed
#include <time.h>      // clock
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static FILE* fptr;
static double sumOfTimes = 0;
 
static void decompress(const char* fname)
{
    size_t cSize;
    void* const cBuff = mallocAndLoadFile_orDie(fname, &cSize);
    /* Read the content size from the frame header. For simplicity we require
     * that it is always present. By default, zstd will write the content size
     * in the header when it is known. If you can't guarantee that the frame
     * content size is always written into the header, either use streaming
     * decompression, or ZSTD_decompressBound().
     */
    unsigned long long const rSize = ZSTD_getFrameContentSize(cBuff, cSize);
    CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
    CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
 
    void* const rBuff = malloc_orDie((size_t)rSize);
 
    /* Decompress.
     * If you are doing many decompressions, you may want to reuse the context
     * and use ZSTD_decompressDCtx(). If you want to set advanced parameters,
     * use ZSTD_DCtx_setParameter().
     */
    clock_t begin = clock();

    size_t const dSize = ZSTD_decompress(rBuff, rSize, cBuff, cSize);
    CHECK_ZSTD(dSize);
    /* When zstd knows the content size, it will error if it doesn't match. */
    CHECK(dSize == rSize, "Impossible because zstd will check this condition!");

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    sumOfTimes += time_spent;
    fprintf(fptr, "%lf s\n", time_spent);
 
    /* success */
    printf("%25s : %6u -> %7u \n", fname, (unsigned)cSize, (unsigned)rSize);
 
    free(rBuff);
    free(cBuff);
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];

    fptr = fopen("../data/simple_decompression.txt", "a"); // the runtimes are stored in this file
    fprintf(fptr, "%s", "\nStarting new session for simple_decompression\n");

 
    if (argc!=2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s FILE\n", exeName);
        return 1;
    }
    int numOfIterations = 10;
    for(int iter = 0; iter < numOfIterations; iter++){
        printf("Iteration-%d: ", iter + 1);
        decompress(argv[1]);
    }
    fprintf(fptr, "Average runtime: %lf s\n", sumOfTimes / numOfIterations);
    fclose(fptr);
    
    printf("%s correctly decoded (in memory). \n", argv[1]);
 
    return 0;
}
