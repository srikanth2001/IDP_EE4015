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
static double compressionRatio;
static size_t compressedSize, headerSize;

size_t min(size_t a, size_t b){
    return (a < b) ? a : b;
}

static void compress(const char* fname, const char* oname)
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
        size_t const cSize = ZSTD_compress(cBuff, cBuffSize, (unsigned char*)fBuff + offset, realSize, 1);
        CHECK_ZSTD(cSize);

        memcpy((unsigned char*)out + outSize, cBuff, cSize);
        header[outSize] = 1;
        outSize += cSize;

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
    
    // Comment this check condition if you are running the benchmarking for multiple dictionaries
    if (argc<2) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }

    for (int u=1; u<argc; u++) {
        const char* inFilename = argv[u];
        char* const outFilename = createOutFilename_orDie(inFilename);
        compress(inFilename, outFilename);       
        free(outFilename);
    }

    return 0;
}
