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
#include <string.h>    // memset, strcat
#include <zstd.h>      // presumes zstd library is installed
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

const int CHUNK_SIZE = 5 * (1 << 20); // Size of each chunk = 5 MB 
 
/* createDict() :
   `dictFileName` is supposed to have been created using `zstd --train` */
static ZSTD_CDict* createCDict_orDie(const char* dictFileName, int cLevel)
{
    size_t dictSize;
    printf("loading dictionary %s \n", dictFileName);
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
    int numOfChunks = (fSize + CHUNK_SIZE - 1) /  CHUNK_SIZE; // ceil(fSize / CHUNK_SIZE)
    void* const out = malloc_orDie(fSize);
    void* const header = malloc_orDie(50 * (numOfChunks + 1)); // For indexes < 10^10

    char buff[40];
    sprintf(buff, "%d ", numOfChunks);
    memcpy((unsigned char*)header, buff, strlen(buff));
    size_t headerSize = strlen(buff);

    for(int chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += CHUNK_SIZE){
        size_t realSize = (size_t)min(CHUNK_SIZE, (int)fSize - offset);
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
    free(fBuff);
    
    saveFile_orDie(oname, header, headerSize, "wb");
    saveFile_orDie(oname, out, outSize, "ab");
    free(out);
    free(header);
    /* success */
    printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)(headerSize + outSize), oname);
}
 
 
static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 20;
    void* outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat(outSpace, filename);
    strcat(outSpace, "_dict_chunks.zst");
    return (char*)outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
    int const cLevel = 3;
 
    if (argc<3) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    /* load dictionary only once */
    const char* const dictName = argv[argc-1];
    ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);
 
    int u;
    for (u=1; u<argc-1; u++) {
        const char* inFilename = argv[u];
        char* const outFilename = createOutFilename_orDie(inFilename);
        compress(inFilename, outFilename, dictPtr);
        free(outFilename);
    }
 
    ZSTD_freeCDict(dictPtr);
    printf("All %u files compressed. \n", argc-2);
    return 0;
}
