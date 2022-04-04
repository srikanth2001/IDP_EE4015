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
#include "../sd_vector.hpp"
#include "../common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()
#include <sdsl/bit_vectors.hpp>

static size_t chunkSize = 5 * (1 << 20); 
static size_t compressedSize, dictSize, headerSize;
 
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

size_t min(size_t a, size_t b){
    return (a < b) ? a : b;
}

static void compress(const char* fname, const char* oname, const ZSTD_CDict* cdict)
{
    size_t fSize, outSize = 0;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    size_t numOfChunks = (fSize + chunkSize - 1ll) /  chunkSize; // ceil(fSize / chunkSize)
    void* const out = malloc_orDie(fSize + 15ll * numOfChunks);

    char hBuff[40];
    headerSize = 0;
    sprintf(hBuff, "%ld %ld ", numOfChunks, fSize);
    filePutContents(oname, (void*)hBuff, strlen(hBuff));
    headerSize += strlen(hBuff);

    sdsl::bit_vector header(fSize, 0);

    for(size_t chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += chunkSize){
        size_t realSize = min(chunkSize, fSize - offset);
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
        header[outSize] = 1;
        outSize += cSize;
    
        ZSTD_freeCCtx(cctx);   /* never fails */
        free(cBuff);
    }

    header.resize(outSize);
    sd_vector<> cHeader(header);
    headerSize += size_in_bytes(cHeader);

    filePutContents(oname, cHeader, true);
    filePutContents(oname, out, outSize, true);

    free(out);
    free(fBuff);
    /* success */
    printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)(headerSize + outSize), oname);
}
 
 
static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 30;
    void* outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat((char*)outSpace, filename);
    strcat((char*)outSpace, "_bv_rlz.bv_rlz");
    return (char*)outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
    int const cLevel = 3;
    
    // Comment this check condition if you are running the benchmarking for multiple dictionaries
    if (argc<3) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    /* load dictionary only once */
    const char* const dictName = argv[argc - 1];
    ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);

    for (int u=1; u<argc-1; u++) {
        const char* inFilename = argv[u];
        char* const outFilename = createOutFilename_orDie(inFilename);
        compress(inFilename, outFilename, dictPtr);       
        free(outFilename);
    }

    return 0;
}
