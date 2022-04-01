#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat, strtol
#include <zstd.h>      // presumes zstd library is installed
#include <time.h>      // clock
#include "../../src/sd_vector.hpp"
#include "../../src/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

static size_t numOfChunks = 0, headerSize = 0, fSize = 0, cSize;
static double sumRATime = 0, sumTotalTime = 0, randomAccessTime = 0, totalTime = 0;
 

void readHeader(const char* fname, void* const inBuff, sd_vector<>& header){
    char delim[2] = " ";
    char* ptr;

    char* token = strtok((char*)inBuff, delim);
    numOfChunks = strtol(token, &ptr, 10);
    headerSize += strlen(token) + 1;

    token = strtok(NULL, delim);
    fSize = strtol(token, &ptr, 10);
    headerSize += strlen(token) + 1;

    readFileToSdVector(header, fname, headerSize);
    headerSize += size_in_bytes(header);
    cSize = header.size();
}
 
static void decompress(const char* fname, const char* oname)
{
    size_t inSize;
    void* const inBuff = mallocAndLoadFile_orDie(fname, &inSize);
    sd_vector<> header;

    readHeader(fname, inBuff, header);

    select_support_sd<> headerSelect(&header);

    // printf("No. of chunks: %d\n", numOfChunks);
    size_t offset = headerSize;
    unsigned long long outSize = 0; 

    void* const out = malloc_orDie(fSize);
    
    randomAccessTime = 0, totalTime = 0;
    clock_t begin = clock();

    for(size_t chunk = 0, pos = offset; chunk < numOfChunks; chunk++){
        clock_t cBegin = clock();
        size_t cBlockSize = (chunk == (numOfChunks - 1ll) ? cSize : headerSelect.select(chunk + 2)) 
                                            - headerSelect.select(chunk + 1);

        // printf("No. of chunks: %d\n", numOfChunks);

        unsigned long long const rSize = ZSTD_getFrameContentSize((unsigned char*)inBuff + pos, cBlockSize);
        CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", fname);
        CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", fname);
        void* const rBuff = malloc_orDie((size_t)rSize); // To store the decompressed data

        size_t const dSize = ZSTD_decompress(rBuff, rSize, (unsigned char*)inBuff + pos, cBlockSize);
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
    totalTime = (end - begin) / (double)CLOCKS_PER_SEC;    
    randomAccessTime /= ((double)numOfChunks * CLOCKS_PER_SEC);
    sumRATime += randomAccessTime;
    sumTotalTime += totalTime;

    saveFile_orDie(oname, out, outSize, "wb");
    free(out);
    free(inBuff);
    /* success */
    // printf("%25s : %6u -> %25s : %7u \n", fname, (unsigned)fSize, oname, (unsigned)outSize);
}

static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL;
    char* outSpace = (char*)malloc_orDie(sizeof(char) * outL);
    memset(outSpace, 0, sizeof(char) * outL);
    for(int i = 0; i < inL - 7; i++) // remove the .bv_rlz part
        outSpace[i] = filename[i];
    return outSpace;
}
 
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
 
    if (argc<2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s [FILES] dictionary\n", exeName);
        return 1;
    }
 
    const int noi = 10;
    
    for(int iter = 0; iter < noi; iter++){
        for (int u=1; u<argc; u++){
            const char* inFilename = argv[u];
            char* const outFilename = createOutFilename_orDie(inFilename);
            numOfChunks = 0, headerSize = 0;
            decompress(inFilename, outFilename);
            free(outFilename);
        }
    }
    printf("Avg. random access time for a chunk: %lf s\n", sumRATime / noi);
    printf("Total time for decompression: %lf s\n\n", sumTotalTime / noi);
    
    // printf("All %u files correctly decoded\n", argc-2);
    return 0;
}