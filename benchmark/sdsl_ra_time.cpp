#include <iostream>
#include <chrono>
#include <random>
#include "../src/common.h"
#include "../src/sd_vector.hpp"
#include <sdsl/bit_vectors.hpp>
#include <string.h>
using namespace std;

const size_t BIT_VECTOR_SIZE = 1ll << 30; // 1GB
char fname[] = "../data/onePos.dat";
static double avgTimeSdsl = 0, avgTimeFile = 0;
static size_t fSize;

// Random no. generator
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

sdsl::bit_vector generateBitVector(double sparsity){
    sdsl::bit_vector b(BIT_VECTOR_SIZE, 0);
    size_t numOfOnes = sparsity * BIT_VECTOR_SIZE;
    void* posList = malloc_orDie(20ll * (numOfOnes + 1ll));

    char sIdx[20];
    size_t offset = 0;
    sprintf(sIdx, "%ld ", numOfOnes);
    memcpy((unsigned char*)posList, sIdx, strlen(sIdx));
    offset += strlen(sIdx);
    
    for(size_t iter = 0; iter < numOfOnes; iter++){
        size_t idx = (size_t)rng() % BIT_VECTOR_SIZE;
        b[idx] = 1;
        sprintf(sIdx, "%ld ", idx);
        memcpy((unsigned char*)posList + offset, sIdx, strlen(sIdx));
        offset += strlen(sIdx);
    }
    saveFile_orDie(fname, posList, offset, "wb");
    free(posList);
    return b;
}

int accessIndexInFile(void* fBuff, size_t reqIdx){
    char delim[2] = " ";
    char* ptr;
    char* token = strtok((char*)fBuff, delim);
    size_t numOfOnes = strtol(token, &ptr, 10);
    // printf("%ld ", numOfOnes);
    size_t currVal;
    for(size_t i = 0; i < numOfOnes; i++){
        token = strtok(NULL, delim);
        if(token != NULL) currVal = strtol(token, &ptr, 10);
        if(currVal == reqIdx)
            return 1;
    }
    return 0;
}   

void measureRATime(sd_vector<>& sdb, void* fBuff){
    const int noi = 100;
    avgTimeSdsl = 0, avgTimeFile = 0;
    for(int iter = 0; iter < noi; iter++){
        size_t reqIdx = (size_t)rng() % BIT_VECTOR_SIZE;

        chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
        sdb[reqIdx];
        chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        avgTimeSdsl += chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

        begin = chrono::high_resolution_clock::now();
        accessIndexInFile(fBuff, reqIdx);
        end = chrono::high_resolution_clock::now();
        avgTimeFile += chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    }
    avgTimeSdsl /= noi;
    avgTimeFile /= noi;
}

int main(){
    const double minSparsity = 0.01, maxSparsity = 0.25;
    const double ds = (maxSparsity - minSparsity) / 5;
    for(double sparsity = minSparsity; sparsity <= maxSparsity; sparsity += ds){
        sdsl::bit_vector b = generateBitVector(sparsity);
        sd_vector<> sdb(b);
        fSize = 0;
        void* fBuff = mallocAndLoadFile_orDie(fname, &fSize);
        measureRATime(sdb, fBuff);

        cout << "For sparsity: " << sparsity * 100.0 << " %\n";
        cout << "Using SDSL:\nCompressed bit vector size: " << sdsl::size_in_bytes(sdb) << " B\n";
        cout << "Avg. random access time: " << avgTimeSdsl / 1000.0 << " microseconds\n\n";
        cout << "Using a lookup file:\nFile size: " << fSize << " B\n";
        cout << "Avg. random access time: " << avgTimeFile / 1000.0 << " microseconds\n\n";
        free(fBuff);
    }
    
    return 0;
}