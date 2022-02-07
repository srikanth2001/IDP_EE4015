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
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t numOfChunks = 0, headerSize = 0;
static double sumRATime = 0, sumTotalTime = 0, randomAccessTime = 0, totalTime = 0;

struct entry{
    size_t cPos, inPos, inSize;
};
 
/* createDict() :
   `dictFileName` is supposed to have been created using `zstd --train` */
static ZSTD_DDict* createDict_orDie(const char* dictFileName)
{
    size_t dictSize;
    // printf("loading dictionary %s \n", dictFileName);
    void* const dictBuffer = mallocAndLoadFile_orDie(dictFileName, &dictSize);
    ZSTD_DDict* const ddict = ZSTD_createDDict(dictBuffer, dictSize);
    CHECK(ddict != NULL, "ZSTD_createDDict() failed!");
    free(dictBuffer);
    return ddict;
}

struct entry* readHeader(void* const cBuff, size_t cSize){
    char delim[2] = " ";
    char* token = strtok((char*)cBuff, delim);
    headerSize += strlen(token) + 1;
    char* ptr;
    numOfChunks = strtol(token, &ptr, 10);

    struct entry* table = (struct entry*)malloc_orDie(sizeof(struct entry) * (numOfChunks + 1));

    for(size_t cnt = 0; cnt < numOfChunks; cnt++){
        token = strtok(NULL, delim);
        table[cnt].cPos = strtol(token, &ptr, 10);
        headerSize += strlen(token) + 1;
        token = strtok(NULL, delim);
        headerSize += strlen(token) + 1;
        table[cnt].inPos = strtol(token, &ptr, 10);
        token = strtok(NULL, delim);
        headerSize += strlen(token) + 1;
        table[cnt].inSize = strtol(token, &ptr, 10);
    }
    return table;
}
 
static void decompress(const char* fname, const char* oname, const ZSTD_DDict* ddict)
{
    size_t cSize;
    void* const cBuff = mallocAndLoadFile_orDie(fname, &cSize);
    
    struct entry* table = readHeader(cBuff, cSize);

    /* Read the content size from the frame header. For simplicity we require
     * that it is always present. By default, zstd will write the content size
     * in the header when it is known. If you can't guarantee that the frame
     * content size is always written into the header, either use streaming
     * decompression, or ZSTD_decompressBound().
     */
 
    /* Check that the dictionary ID matches.
     * If a non-zstd dictionary is used, then both will be zero.
     * By default zstd always writes the dictionary ID into the frame.
     * Zstd will check if there is a dictionary ID mismatch as well.
     */
    unsigned const expectedDictID = ZSTD_getDictID_fromDDict(ddict);
    unsigned const actualDictID = ZSTD_getDictID_fromFrame((unsigned char*)cBuff + headerSize, cSize);
    CHECK(actualDictID == expectedDictID,
          "DictID mismatch: expected %u got %u",
          expectedDictID,
          actualDictID); 
 
    /* Decompress using the dictionary.
     * If you need to control the decompression parameters, then use the
     * advanced API: ZSTD_DCtx_setParameter(), ZSTD_DCtx_refDDict(), and
     * ZSTD_decompressDCtx().
     */
    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");

    printf("No. of chunks: %d\n", numOfChunks);
    size_t offset = headerSize;
    unsigned long long outSize = 0; 
    table[numOfChunks] = (struct entry){cSize - offset, table[numOfChunks - 1].inPos + table[numOfChunks - 1].inSize, 0};

    void* const out = malloc_orDie(table[numOfChunks - 1].inPos + table[numOfChunks - 1].inSize); // Assuming the max. compression ratio is 4
    
    randomAccessTime = 0, totalTime = 0;
    clock_t begin = clock();

    for(size_t chunk = 0, pos = offset + table[0].cPos; chunk < numOfChunks; chunk++){
        size_t cBlockSize = table[chunk + 1].cPos - table[chunk].cPos;
        clock_t cBegin = clock();
        unsigned long long const rSize = ZSTD_getFrameContentSize((unsigned char*)cBuff + pos, cBlockSize);
        CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
        CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
        void* const rBuff = malloc_orDie((size_t)rSize); // To store the decompressed data

        size_t const dSize = ZSTD_decompress_usingDDict(dctx, rBuff, rSize, (unsigned char*)cBuff + pos, cBlockSize, ddict);
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
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;    
    randomAccessTime /= ((double)numOfChunks * CLOCKS_PER_SEC);
    sumRATime += randomAccessTime;
    sumTotalTime += totalTime;

    saveFile_orDie(oname, out, outSize, "wb");
    ZSTD_freeDCtx(dctx);
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
 
    if (argc<3) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    /* load dictionary only once */
    const char* const dictName = argv[argc-1];
    ZSTD_DDict* const dictPtr = createDict_orDie(dictName);
 
    const int noi = 10;
    
    for(int iter = 0; iter < noi; iter++){
        for (int u=1; u<argc-1; u++){
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            numOfChunks = 0, headerSize = 0;
            decompress(inFilename, outFilename, dictPtr);
            free(outFilename);
        }
    }
    printf("Avg. random access time for a chunk: %lf s\n", sumRATime / noi);
    printf("Total time for decompression: %lf s\n\n", sumTotalTime / noi);
 
    ZSTD_freeDDict(dictPtr);
    // printf("All %u files correctly decoded\n", argc-2);
    return 0;
}