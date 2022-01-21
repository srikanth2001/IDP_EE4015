#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat
#include <zstd.h>      // presumes zstd and sdsl libraries are installed
#include <fstream>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include "../common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

using namespace sdsl;
using namespace std;

const int CHUNK_SIZE = 5 * (1 << 20); // Size of each chunk = 5 MB 
const double eps = 0.01;
 
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

void filePutContents(const char* fname, const void* ptr, const size_t size, bool append = false){
    ofstream out;
    if (append)
        out.open(fname, std::ios_base::app);
    else
        out.open(fname);

    if( !out ) { // file couldn't be opened
        printf("Error: file could not be opened\n");
        exit(1);
    }
    
    out.write((const char*)ptr, size);
    out.close();
}

void filePutContents(const char* fname, const sd_vector<>& sdb, bool append = false){
    ofstream out;
    if (append)
        out.open(fname, ios_base::app);
    else
        out.open(fname);

    if( !out ) { // file couldn't be opened
        printf("Error: file could not be opened\n");
        exit(1);
    }
    
    sdb.serialize(out);
    out.close();
}

static void compress(const char* fname, const char* oname, const ZSTD_CDict* cdict)
{
    size_t fSize;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    int numOfChunks = (fSize + CHUNK_SIZE - 1) /  CHUNK_SIZE; // ceil(fSize / CHUNK_SIZE)

    void* const header = malloc_orDie(50 * (numOfChunks + 1)); // Increase the header capacity for larger files

    // Compressing the file to compute entropy of the file X
    printf("Computing entropy of the file.........\n");
    size_t cBuffSize = ZSTD_compressBound(fSize);
    void* cBuff = malloc_orDie(cBuffSize);
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");
    size_t cSize = ZSTD_compress_usingCDict(cctx, cBuff, cBuffSize, fBuff, fSize, cdict);
    CHECK_ZSTD(cSize);
    free(cBuff);

    const double H_X = (double)cSize / fSize;
    printf("Entropy of the file %s: %lf\n", fname, H_X);

    // Threshold length = b * H(X) * (1 + eps)
    const size_t threshold = (size_t)(CHUNK_SIZE * (1 + eps) * H_X);

    char hBuff[40];
    sprintf(hBuff, "%d %ld %d ", numOfChunks, threshold, CHUNK_SIZE);
    memcpy((unsigned char*)header, hBuff, strlen(hBuff));
    int headerSize = strlen(hBuff);
    
    size_t denseSize = numOfChunks * threshold, sparseSize = numOfChunks * CHUNK_SIZE;
    void* const denseStream = malloc_orDie(denseSize);
    bit_vector sparseStream(8 * sparseSize, 0);
    memset((unsigned char*)denseStream, 0, sizeof(unsigned char) * denseSize);
    size_t denseOffset = 0, sparseOffset = 0;

    for(int chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += CHUNK_SIZE){
        size_t realSize = (size_t)min(CHUNK_SIZE, (int)fSize - offset);
        // printf("%ld ", realSize);
        cBuffSize = ZSTD_compressBound(realSize);
        cBuff = malloc_orDie(cBuffSize);
    
        // Compress the chunk to check its typicality
        cSize = ZSTD_compress_usingCDict(cctx, cBuff, cBuffSize, (unsigned char*)fBuff + offset, realSize, cdict);
        CHECK_ZSTD(cSize);

        if(cSize <= threshold){
           memcpy((unsigned char*)denseStream + denseOffset, cBuff, cSize);
        }
        else{
            memcpy((unsigned char*)denseStream + denseOffset, cBuff, threshold);
            copyToBitVector(sparseStream, (unsigned char*)cBuff + threshold, sparseOffset, cSize - threshold);
        }

        denseOffset += threshold;
        sparseOffset += CHUNK_SIZE;

        sprintf(hBuff, "%ld ", cSize); // Store the compressed size for each chunk in the header.
        memcpy((unsigned char*)header + headerSize, hBuff, strlen(hBuff));
        headerSize += strlen(hBuff);

        free(cBuff);
    }

    printf("Compressing the sparse stream using bit-vector-compressor........\n");

    sd_vector<> cSparse(sparseStream);

    // Write contents to file
    filePutContents(oname, header, headerSize);
    filePutContents(oname, denseStream, denseSize, true);
    filePutContents(oname, cSparse, true);
    ZSTD_freeCCtx(cctx);   /* never fails */
    free(fBuff);
    free(denseStream);
    free(header);
    /* success */
    printf("%25s : %6ld -> %7ld - %s \n", fname, fSize, headerSize + denseSize + size_in_bytes(cSparse), oname);
}
 
 
static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 20;
    char* outSpace = (char*)malloc_orDie(outL * sizeof(char));
    memset(outSpace, 0, sizeof(char) * outL);
    strcat(outSpace, filename);
    strcat(outSpace, "_ds.ds");
    return outSpace;
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
        // printf("%s", outFilename);
        compress(inFilename, outFilename, dictPtr);
        free(outFilename);
    }
 
    ZSTD_freeCDict(dictPtr);
    printf("All %u files compressed. \n", argc-2);
    return 0;
}
