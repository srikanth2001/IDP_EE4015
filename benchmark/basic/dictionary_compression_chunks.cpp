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
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t chunkSize; 
static double sumOfTimes = 0, compressionRatio;
static size_t compressedSize, dictSize;
 
/* createDict() :
   `dictFileName` is supposed to have been created using `zstd --train` */
static ZSTD_CDict* createCDict_orDie(const char* dictFileName, int cLevel)
{
    // printf("loading dictionary %s \n", dictFileName);
    void* const dictBuffer = mallocAndLoadFile_orDie(dictFileName, &dictSize);
    ZSTD_CDict* const cdict = ZSTD_createCDict(dictBuffer, dictSize, cLevel);
    CHECK(cdict != NULL, "ZSTD_createCDict() failed!");
    free(dictBuffer);
    return cdict;
}

int min(int a, int b){
    return (a < b) ? a : b;
}

static void compress(const char* fname, const char* oname, const ZSTD_CDict* cdict)
{
    size_t fSize, outSize = 0;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    int numOfChunks = (fSize + chunkSize - 1) /  chunkSize; // ceil(fSize / chunkSize)
    void* const out = malloc_orDie(fSize);
    void* const header = malloc_orDie(50 * (numOfChunks + 1)); // For indexes < 10^10

    char buff[40];
    sprintf(buff, "%d ", numOfChunks);
    memcpy((unsigned char*)header, buff, strlen(buff));
    size_t headerSize = strlen(buff);

    clock_t begin = clock();

    for(int chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += chunkSize){
        size_t realSize = (size_t)min(chunkSize, (int)fSize - offset);
        // printf("%ld ", realSize);
        
        size_t const cBuffSize = ZSTD_compressBound(realSize);
        void* const cBuff = malloc_orDie(cBuffSize);
    
        /* Compress using the dictionary.
        * This function writes the dictionary id, and content size into the header.
        * But, it doesn't use a checksum. You can control these options using the
        * advanced API: ZSTD_CCtx_setParameter(), ZSTD_CCtx_refCDict(),
        * and ZSTD_compress2().
        */
        ZSTD_CCtx* const cctx = ZSTD_createCCtx();
        CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");
        size_t const cSize = ZSTD_compress_usingCDict(cctx, cBuff, cBuffSize, (unsigned char*)fBuff + offset, realSize, cdict);
        CHECK_ZSTD(cSize);

        memcpy((unsigned char*)out + outSize, cBuff, cSize);

        sprintf(buff, "%ld %d %ld ", outSize, offset, realSize); // header entries for each chunk
        memcpy((unsigned char*)header + headerSize, buff, strlen(buff));
        
        headerSize += strlen(buff);
        outSize += cSize;
    
        ZSTD_freeCCtx(cctx);   /* never fails */
        free(cBuff);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    sumOfTimes += time_spent;
    compressedSize = outSize;
    compressionRatio = fSize / (double)outSize;

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
    strcat((char*)outSpace, "_dict_chunks.zst");
    return (char*)outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
    int const cLevel = 3;
    
    // Comment this check condition if you are running the benchmarking for multiple dictionaries
    if (argc<4) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    /* load dictionary only once */
    const char* const dictName = argv[argc - 2];
    char* ptr;
    // The below code is for compressing using the chunk size specified in argv
    chunkSize = (size_t)strtol(argv[argc - 1], &ptr, 10);
    ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);

    const int noi = 10;
    
    for(int iter = 0; iter < noi; iter++){
        for (int u=1; u<argc-2; u++) {
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            compress(inFilename, outFilename, dictPtr);       
            free(outFilename);
        }
    }
    printf("Compressed file size: %ld B\n", compressedSize);
    printf("Compression ratio: %lf\n", compressionRatio);
    printf("Avg. compression time: %lf s\n", sumOfTimes / noi);
 

    // The below code is for compressing using different chunk sizes
    // for (int u=1; u<argc-1; u++) {
    //     const char* inFilename = argv[u];
    //     char* const outFilename = createOutFilename_orDie(inFilename);

    //     const int minSize = 50 * (1 << 10), maxSize = 5 * (1 << 20);
    //     const int dc = (maxSize - minSize) / 10;
    //     const int noi = 10;
    //     for(chunkSize = minSize; chunkSize <= minSize; chunkSize += dc){
    //         printf("For chunk size = : %ld B\n", chunkSize);
    //         sumOfTimes = 0.0;
    //         for(int iter = 0; iter < noi; iter++){
    //             compress(inFilename, outFilename, dictPtr);
    //         }
    //         printf("Compressed file size: %ld B\n", compressedSize);
    //         printf("Average runtime: %lf s\n\n", sumOfTimes / noi);
    //     }
        
    //     free(outFilename);
    // }

    // The below code is for compressing using different dictionaries
    // for (u=1; u<argc; u++) {
    //     char* dictArray[] = {"../data/dict_1.zdict", "../data/dict_2.zdict", "../data/dict_3.zdict", "../data/dict_4.zdict",
    //                         "../data/dict_5.zdict", "../data/dict_6.zdict", "../data/dict_7.zdict", "../data/dict_8.zdict",
    //                         "../data/dict_9.zdict", "../data/dict_10.zdict", "../data/dict_11.zdict"};

    //     for(int d = 0; d < 11; d++){ 
    //         char* dictName = dictArray[d];                   
    //         ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);
    //         const char* inFilename = argv[u];
    //         char* const outFilename = createOutFilename_orDie(inFilename);

    //         const int noi = 10;
    //         printf("For dictionary size = : %ld B\n", dictSize);
    //         sumOfTimes = 0.0;
    //         for(int iter = 0; iter < noi; iter++){
    //             compress(inFilename, outFilename, dictPtr);
    //         }
    //         printf("Compressed file size: %ld B\n", compressedSize);
    //         printf("Average runtime: %lf s\n\n", sumOfTimes / noi);
            
    //         free(outFilename);
    //         ZSTD_freeCDict(dictPtr);
    //     }
    // }

    return 0;
}
