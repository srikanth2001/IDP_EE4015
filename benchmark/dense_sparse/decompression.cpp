#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat
#include <zstd.h>      // presumes zstd and sdsl libraries are installed
#include <fstream>
#include <time.h>
#include <sdsl/bit_vectors.hpp>
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

using namespace sdsl;
using namespace std;

static size_t numOfChunks, headerSize = 0;
static size_t threshold, chunkSize;
static double sumRATime = 0, sumTotalTime = 0, randomAccessTime = 0, totalTime = 0;
 
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

void readHeader(size_t** cLengths, void* const cBuff, size_t cSize){
    char delim[2] = " ";
    char* token = strtok((char*)cBuff, delim);
    headerSize += strlen(token) + 1;
    char* ptr;
    numOfChunks = (int)strtol(token, &ptr, 10);

    token = strtok(NULL, delim);
    headerSize += strlen(token) + 1;
    threshold = (size_t)strtol(token, &ptr, 10);

    token = strtok(NULL, delim);
    headerSize += strlen(token) + 1;
    chunkSize = (size_t)strtol(token, &ptr, 10);

    *cLengths = (size_t*)malloc_orDie(sizeof(size_t) * (numOfChunks + 1));
    for(int i = 0; i < numOfChunks; i++){
        token = strtok(NULL, delim);
        headerSize += strlen(token) + 1;
        (*cLengths)[i] = (size_t)strtol(token, &ptr, 10);
    }
}

void fileReadToBitVector(sd_vector<>& cSparse, const char* fname, size_t offset){
    ifstream in(fname, ifstream::binary);
    if( !in ) { // file couldn't be opened
        printf("Error: file could not be opened\n");
        exit(1);
    }
    in.seekg(offset);
    cSparse.load(in);
    in.close();
}
 
static void decompress(const char* fname, const char* oname, const ZSTD_DDict* ddict)
{
    size_t cSize;
    void* const cBuff = mallocAndLoadFile_orDie(fname, &cSize);

    // cLengths stores the sizes of the compressed blocks
    size_t* cLengths;
    readHeader(&cLengths, cBuff, cSize);
    int offset = headerSize;
    const size_t denseSize = numOfChunks * threshold;
    
    /* Check that the dictionary ID matches.
     * If a non-zstd dictionary is used, then both will be zero.
     * By default zstd always writes the dictionary ID into the frame.
     * Zstd will check if there is a dictionary ID mismatch as well.
     */
    unsigned const expectedDictID = ZSTD_getDictID_fromDDict(ddict);
    unsigned const actualDictID = ZSTD_getDictID_fromFrame((unsigned char*)cBuff + offset, cSize);
    CHECK(actualDictID == expectedDictID,
          "DictID mismatch: expected %u got %u",
          expectedDictID,
          actualDictID); 
          
    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");

    size_t outSize = 0; 

    sd_vector<> cSparse;
    fileReadToBitVector(cSparse, fname, offset + denseSize);
    void* const out = malloc_orDie(numOfChunks * chunkSize);

    randomAccessTime = 0, totalTime = 0;
    clock_t begin = clock();

    for(int chunk = 0; chunk < numOfChunks; chunk++){
        // Size of the compressed block (obtained from the header)
        size_t cBlockSize = cLengths[chunk];
        // To store together the dense and sparse blocks of the current chunk
        clock_t cBegin = clock();
        void* const buff = malloc_orDie(cBlockSize);
        if(cBlockSize <= threshold){
            // Copy the dense stream 
            memcpy(buff, (unsigned char*)cBuff + offset + chunk * threshold, cBlockSize);
        }
        else{
            // Copy the dense stream 
            memcpy(buff, (unsigned char*)cBuff + offset + chunk * threshold, threshold);
            // Copy the sparse stream
            copyToArray((unsigned char*)buff + threshold, cSparse, chunk * chunkSize, cBlockSize - threshold);
        }

        // Decompress the block
        const size_t rSize = ZSTD_getFrameContentSize(buff, cBlockSize);
        CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
        CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
        void* const rBuff = malloc_orDie(rSize); // To store the decompressed data
        size_t const dSize = ZSTD_decompress_usingDDict(dctx, rBuff, rSize, buff, cBlockSize, ddict);
        CHECK_ZSTD(dSize);
        /* When zstd knows the content size, it will error if it doesn't match. */
        CHECK(dSize == rSize, "Impossible because zstd will check this condition!");

        clock_t cEnd = clock();
        randomAccessTime += (double)(cEnd - cBegin);

        memcpy((unsigned char*)out + outSize, rBuff, rSize); // Copying rBuff to out
        outSize += rSize;
        free(rBuff);
        free(buff);
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
    free(cLengths);
    /* success */
    // printf("%25s : %6u -> %25s : %7u \n", fname, (unsigned)cSize, oname, (unsigned)outSize);
}

static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL;
    char* outSpace = (char*)malloc_orDie(sizeof(char) * outL);
    memset(outSpace, 0, sizeof(char) * outL);
    for(int i = 0; i < (int)inL - 3; i++) // remove the .ds part
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
    
    const int noi = 5;
    for(int iter = 0; iter < noi; iter++){
        int u;
        for (u=1; u<argc-1; u++){
            numOfChunks = 0, headerSize = 0;
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            decompress(inFilename, outFilename, dictPtr);
            free(outFilename);
        }
    }
    printf("Avg. random access time for a chunk: %lf s\n", sumRATime / noi);
    printf("Avg. decompression time: %lf s\n\n", sumTotalTime / noi);

    ZSTD_freeDDict(dictPtr);
    // printf("All %u files correctly decoded\n", argc-2);
    return 0;
}