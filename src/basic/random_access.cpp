// TODO: How to get the input indices from the lookup table data
/* For decompression with random access. The code is almost the same as dictionary_decompression_chunks.c except 
for the random access part. */
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
#include <string.h>    // strtok
#include <zstd.h>      // presumes zstd library is installed
#include "../common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t numOfChunks, headerSize = 0;
 
struct entry{ // look-up table entry
    size_t cPos, inPos, inSize;
};
 
/* createDict() :
   `dictFileName` is supposed to have been created using `zstd --train` */
static ZSTD_DDict* createDict_orDie(const char* dictFileName)
{
    size_t dictSize;
    printf("loading dictionary %s \n", dictFileName);
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

    for(int cnt = 0; cnt < numOfChunks; cnt++){
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

int getChunkIndex(size_t idx, struct entry* table){ // Binary search for the chunk's index
    size_t l = 0, r = numOfChunks - 1;
    while(l <= r){
        size_t m = l + (r - l) / 2;
        if(idx >= table[m].inPos && idx < table[m + 1].inPos)
            return (int)m;
        if(idx > table[m].inPos) l = m + 1;
        else r = m - 1;
    }
    return -1;
}
 
static void decompress(const char* fname, size_t start, size_t end, const ZSTD_DDict* ddict)
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
    unsigned const actualDictID = ZSTD_getDictID_fromFrame(cBuff + headerSize, cSize);
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

    table[numOfChunks] = (struct entry){cSize - headerSize, table[numOfChunks - 1].inPos + table[numOfChunks - 1].inSize, 0};

    int startChunk = getChunkIndex(start, table), endChunk = getChunkIndex(end, table); 
    // printf("%d %d\n", startChunk, endChunk);
    if(startChunk == -1 || endChunk == -1){
        printf("Invalid start or end positions\n");
        exit(1);
    }

    size_t outSize = 0;
    void* const out = malloc_orDie(end - start + 10); 
    memset(out, 0, end - start + 10);
    size_t pos = headerSize + table[startChunk].cPos;

    for(size_t chunk = startChunk; chunk <= endChunk; chunk++){
        size_t chunkSize = table[chunk + 1].cPos - table[chunk].cPos;
        unsigned long long const rSize = ZSTD_getFrameContentSize((unsigned char*)cBuff + pos, chunkSize);
        CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
        CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
        void* const rBuff = malloc_orDie((size_t)rSize); // To store the decompressed data

        size_t const dSize = ZSTD_decompress_usingDDict(dctx, rBuff, rSize, (unsigned char*)cBuff + pos, chunkSize, ddict);
        CHECK_ZSTD(dSize);
        /* When zstd knows the content size, it will error if it doesn't match. */
        CHECK(dSize == rSize, "Impossible because zstd will check this condition!");

        if(startChunk == endChunk){
            unsigned long long notReql = start - table[chunk].inPos, notReqr = table[chunk + 1].inPos - end;
            memcpy((unsigned char*)out + outSize, rBuff + notReql, rSize - notReql - notReqr); // Copying rBuff to out
            outSize += rSize - notReql - notReqr;
        }
        else if(chunk == startChunk){
            unsigned long long notReq = start - table[chunk].inPos;
            memcpy((unsigned char*)out + outSize, rBuff + notReq, rSize - notReq); // Copying rBuff to out
            outSize += rSize - notReq;
        }
        else if(chunk == endChunk){
            unsigned long long notReq = table[chunk + 1].inPos - end;
            memcpy((unsigned char*)out + outSize, rBuff, rSize - notReq); // Copying rBuff to out
            outSize += rSize - notReq;
        }
        else{
            memcpy((unsigned char*)out + outSize, rBuff, rSize); // Copying rBuff to out
            outSize += rSize; 
        }
        pos += chunkSize;
        free(rBuff);
    }

    printf("Decompression successful!\nThe decompressed string is:\n%s\n", (unsigned char*)out);
    ZSTD_freeDCtx(dctx);
    free(cBuff);
    free(table);
    free(out);
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
 
    int u;
    for (u=1; u<argc-1; u++){
        const char* inFilename = argv[u];
        int start = 0, end = 100; // start and end indices of the target string
        // printf("%d %d\n", start, end);
        // char* const outFilename = createOutFilename_orDie(inFilename);
        decompress(inFilename, start, end, dictPtr);
        // free(outFilename);
    }
 
    ZSTD_freeDDict(dictPtr);
    // printf("All %u files correctly decoded (in memory) \n", argc-2);
    return 0;
}