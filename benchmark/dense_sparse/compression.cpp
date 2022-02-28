#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat, strtol
#include <zstd.h>      // presumes zstd and sdsl libraries are installed
#include <time.h>
#include "../../src/sd_vector.hpp"
#include <sdsl/bit_vectors.hpp>
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

using namespace std;

static size_t chunkSize; 
const double eps = 0.01;
static double sumTotalTime = 0, totalTime = 0, compressionRatio;
static size_t compressedSize, denseSize, cSparseSize;
 
/* createDict() :
   `dictFileName` is supposed to have been created using `zstd --train` */
static ZSTD_CDict* createCDict_orDie(const char* dictFileName, int cLevel)
{
    size_t dictSize;
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
    size_t fSize;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    size_t numOfChunks = (fSize + chunkSize - 1ll) /  chunkSize; // ceil(fSize / chunkSize)

    void* const header = malloc_orDie(50 * (numOfChunks + 1)); // Increase the header capacity for larger files

    // Compressing the file to compute entropy of the file X
    // printf("Computing entropy of the file.........\n");
    size_t cBuffSize = ZSTD_compressBound(fSize);
    void* cBuff = malloc_orDie(cBuffSize);
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");
    size_t cSize = ZSTD_compress_usingCDict(cctx, cBuff, cBuffSize, fBuff, fSize, cdict);
    CHECK_ZSTD(cSize);
    free(cBuff);

    const double H_X = (double)cSize / fSize;
    // printf("Entropy of the file %s: %lf\n", fname, H_X);

    // Threshold length = b * H(X) * (1 + eps)
    const size_t threshold = (size_t)(chunkSize * (1 + eps) * H_X);

    char hBuff[40];
    sprintf(hBuff, "%ld %ld %ld ", numOfChunks, threshold, chunkSize);
    memcpy((unsigned char*)header, hBuff, strlen(hBuff));
    size_t headerSize = strlen(hBuff);
    
    denseSize = numOfChunks * threshold;
    const size_t sparseSize = numOfChunks * chunkSize;
    void* const denseStream = malloc_orDie(denseSize);
    sdsl::bit_vector sparseStream(8ll * sparseSize, 0);
    memset((unsigned char*)denseStream, 0, sizeof(unsigned char) * denseSize);
    size_t denseOffset = 0, sparseOffset = 0;

    totalTime = 0;
    clock_t begin = clock();

    for(size_t chunk = 0, offset = 0; chunk < numOfChunks; chunk++, offset += chunkSize){
        size_t realSize = min(chunkSize, fSize - offset);
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
            copyArrayToBitVector(sparseStream, (unsigned char*)cBuff + threshold, sparseOffset, cSize - threshold);
        }

        denseOffset += threshold;
        sparseOffset += chunkSize;

        sprintf(hBuff, "%ld ", cSize); // Store the compressed size for each chunk in the header.
        memcpy((unsigned char*)header + headerSize, hBuff, strlen(hBuff));
        headerSize += strlen(hBuff);

        free(cBuff);
    }

    // printf("Compressing the sparse stream using bit-vector-compressor........\n");

    sd_vector<> cSparse(sparseStream);
    clock_t end = clock();
    totalTime = (end - begin) / (double)CLOCKS_PER_SEC;
    sumTotalTime += totalTime;
    cSparseSize = size_in_bytes(cSparse);
    compressedSize = denseSize + cSparseSize;
    compressionRatio = fSize / (double)compressedSize;
    
    // Write contents to file
    filePutContents(oname, header, headerSize);
    filePutContents(oname, denseStream, denseSize, true);
    filePutContents(oname, cSparse, true);
    ZSTD_freeCCtx(cctx);   /* never fails */
    free(fBuff);
    free(denseStream);
    free(header);
    /* success */
    // printf("%25s : %6ld -> %7ld - %s \n", fname, fSize, headerSize + denseSize + size_in_bytes(cSparse), oname);
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
 
    if (argc<4) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    /* load dictionary only once */
    const char* const dictName = argv[argc-2];
    char* ptr;
    chunkSize = (size_t)strtol(argv[argc-1], &ptr, 10);
    ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);
    
    const int noi = 5;
    int u;
    for(int iter = 0; iter < noi; iter++){
        for (u=1; u<argc-2; u++) {
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            // printf("%s", outFilename);
            compress(inFilename, outFilename, dictPtr);
            free(outFilename);
        }
    }
    printf("Compressed file size: %ld \nCompression ratio: %lf\n", compressedSize, compressionRatio);
    printf("Dense stream size: %ld\nCompressed Sparse stream size: %ld\n", denseSize, cSparseSize);
    printf("Avg. compression time: %lf s\n", sumTotalTime / noi);
 
    ZSTD_freeCDict(dictPtr);

    return 0;
}
