/*
 * Copyright (c) Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */
 
/*
 * This header file has common utility functions used in examples.
 */
#ifndef COMMON_H
#define COMMON_H
 
#include <stdlib.h>    // malloc, free, exit
#include <stdio.h>     // fprintf, perror, fopen, etc.
#include <string.h>    // strerror
#include <errno.h>     // errno
#include <sys/stat.h>  // stat
#include <zstd.h>
#include "sd_vector.hpp"
#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include <fstream>

using namespace std;
 
/*
 * Define the returned error code from utility functions.
 */
typedef enum {
    ERROR_fsize = 1,
    ERROR_fopen = 2,
    ERROR_fclose = 3,
    ERROR_fread = 4,
    ERROR_fwrite = 5,
    ERROR_loadFile = 6,
    ERROR_saveFile = 7,
    ERROR_malloc = 8,
    ERROR_largeFile = 9,
} COMMON_ErrorCode;
 
/*! CHECK
 * Check that the condition holds. If it doesn't print a message and die.
 */
#define CHECK(cond, ...)                        \
    do {                                        \
        if (!(cond)) {                          \
            fprintf(stderr,                     \
                    "%s:%d CHECK(%s) failed: ", \
                    __FILE__,                   \
                    __LINE__,                   \
                    #cond);                     \
            fprintf(stderr, "" __VA_ARGS__);    \
            fprintf(stderr, "\n");              \
            exit(1);                            \
        }                                       \
    } while (0)
 
/*! CHECK_ZSTD
 * Check the zstd error code and die if an error occurred after printing a
 * message.
 */
#define CHECK_ZSTD(fn, ...)                                      \
    do {                                                         \
        size_t const err = (fn);                                 \
        CHECK(!ZSTD_isError(err), "%s", ZSTD_getErrorName(err)); \
    } while (0)
 
/*! fsize_orDie() :
 * Get the size of a given file path.
 *
 * @return The size of a given file path.
 */
static size_t fsize_orDie(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0) {
        /* error */
        perror(filename);
        exit(ERROR_fsize);
    }
 
    off_t const fileSize = st.st_size;
    size_t const size = (size_t)fileSize;
    /* 1. fileSize should be non-negative,
     * 2. if off_t -> size_t type conversion results in discrepancy,
     *    the file size is too large for type size_t.
     */
    if ((fileSize < 0) || (fileSize != (off_t)size)) {
        fprintf(stderr, "%s : filesize too large \n", filename);
        exit(ERROR_largeFile);
    }
    return size;
}
 
/*! fopen_orDie() :
 * Open a file using given file path and open option.
 *
 * @return If successful this function will return a FILE pointer to an
 * opened file otherwise it sends an error to stderr and exits.
 */
static FILE* fopen_orDie(const char *filename, const char *instruction)
{
    FILE* const inFile = fopen(filename, instruction);
    if (inFile) return inFile;
    /* error */
    perror(filename);
    exit(ERROR_fopen);
}
 
/*! fclose_orDie() :
 * Close an opened file using given FILE pointer.
 */
static void fclose_orDie(FILE* file)
{
    if (!fclose(file)) { return; };
    /* error */
    perror("fclose");
    exit(ERROR_fclose);
}
 
/*! fread_orDie() :
 *
 * Read sizeToRead bytes from a given file, storing them at the
 * location given by buffer.
 *
 * @return The number of bytes read.
 */
static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file)
{
    size_t const readSize = fread(buffer, 1, sizeToRead, file);
    if (readSize == sizeToRead) return readSize;   /* good */
    if (feof(file)) return readSize;   /* good, reached end of file */
    /* error */
    perror("fread");
    exit(ERROR_fread);
}
 
/*! fwrite_orDie() :
 *
 * Write sizeToWrite bytes to a file pointed to by file, obtaining
 * them from a location given by buffer.
 *
 * Note: This function will send an error to stderr and exit if it
 * cannot write data to the given file pointer.
 *
 * @return The number of bytes written.
 */
static size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file)
{
    size_t const writtenSize = fwrite(buffer, 1, sizeToWrite, file);
    if (writtenSize == sizeToWrite) return sizeToWrite;   /* good */
    /* error */
    perror("fwrite");
    exit(ERROR_fwrite);
}
 
/*! malloc_orDie() :
 * Allocate memory.
 *
 * @return If successful this function returns a pointer to allo-
 * cated memory.  If there is an error, this function will send that
 * error to stderr and exit.
 */
static void* malloc_orDie(size_t size)
{
    void* const buff = malloc(size);
    if (buff) return buff;
    /* error */
    perror("malloc");
    exit(ERROR_malloc);
}
 
/*! loadFile_orDie() :
 * load file into buffer (memory).
 *
 * Note: This function will send an error to stderr and exit if it
 * cannot read data from the given file path.
 *
 * @return If successful this function will load file into buffer and
 * return file size, otherwise it will printout an error to stderr and exit.
 */
static size_t loadFile_orDie(const char* fileName, void* buffer, size_t bufferSize)
{
    size_t const fileSize = fsize_orDie(fileName);
    CHECK(fileSize <= bufferSize, "File too large!");
 
    FILE* const inFile = fopen_orDie(fileName, "rb");
    size_t const readSize = fread(buffer, 1, fileSize, inFile);
    if (readSize != (size_t)fileSize) {
        fprintf(stderr, "fread: %s : %s \n", fileName, strerror(errno));
        exit(ERROR_fread);
    }
    fclose(inFile);  /* can't fail, read only */
    return fileSize;
}
 
/*! mallocAndLoadFile_orDie() :
 * allocate memory buffer and then load file into it.
 *
 * Note: This function will send an error to stderr and exit if memory allocation
 * fails or it cannot read data from the given file path.
 *
 * @return If successful this function will return buffer and bufferSize(=fileSize),
 * otherwise it will printout an error to stderr and exit.
 */
static void* mallocAndLoadFile_orDie(const char* fileName, size_t* bufferSize) {
    size_t const fileSize = fsize_orDie(fileName);
    *bufferSize = fileSize;
    void* const buffer = malloc_orDie(*bufferSize);
    loadFile_orDie(fileName, buffer, *bufferSize);
    return buffer;
}
 
/*! saveFile_orDie() :
 *
 * Save buffSize bytes to a given file path, obtaining them from a location pointed
 * to by buff.
 *
 * Note: This function will send an error to stderr and exit if it
 * cannot write to a given file.
 */
static void saveFile_orDie(const char* fileName, const void* buff, size_t buffSize, const char* mode)
{
    FILE* const oFile = fopen_orDie(fileName, mode);
    size_t const wSize = fwrite(buff, 1, buffSize, oFile);
    if (wSize != (size_t)buffSize) {
        fprintf(stderr, "fwrite: %s : %s \n", fileName, strerror(errno));
        exit(ERROR_fwrite);
    }
    if (fclose(oFile)) {
        perror(fileName);
        exit(ERROR_fclose);
    }
}

// Writes the contents of an array to the file using file-stream
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

// Writes the contents of sd_vector to the file using file-stream
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

// Read file contents to an sd_vector
void readFileToSdVector(sd_vector<>& cSparse, const char* fname, size_t offset){
    ifstream in(fname, ifstream::binary);
    if( !in ) { // file couldn't be opened
        printf("Error: file could not be opened\n");
        exit(1);
    }
    in.seekg(offset);
    cSparse.load(in);
    in.close();
}

// Copy contents from an array to a bit-vector
void copyArrayToBitVector(sdsl::bit_vector& b, unsigned char* data, size_t bOffset, size_t size){
    for(size_t i = 0; i < size; i++){
        unsigned char currByte = *(data + i);
        for(int pos = 0; pos < 8; pos++){
            b[8 * (i + bOffset) + pos] = (bool)((currByte >> (7 - pos)) & 1);
        }
    }
}

// Copy contents of bit vector to an array
void copyBitVectorToArray(unsigned char* data, sdsl::bit_vector& b){
    size_t dataSize = (b.size() + 7) / 8; // Size of b in bytes
    for(size_t i = 0; i < dataSize; i++){
        unsigned char currByte = 0;
        for(int pos = 0; pos < 8; pos++){
            currByte |= (unsigned char)(b[8ll * i + pos] * (1 << (7 - pos)));
        }
        *(data + i) = currByte;
    }
}

// Copy contents from an sd vector's slice sdb[8 * bOffset .... 8 * (bOffSet + size - 1)] to an array
void copySdVectorSliceToArrayOpt(unsigned char* data, sd_vector<>& sdb, size_t bOffset, size_t sizeInBytes){
    sdsl::bit_vector b = sdb.get(8ll * bOffset, 8ll * sizeInBytes);
    copyBitVectorToArray(data, b);
}

void copySdVectorSliceToArray(unsigned char* data, sd_vector<>& sdb, size_t bOffset, size_t sizeInBytes){
    for(size_t i = 0; i < sizeInBytes; i++){
        unsigned char currByte = 0;
        for(int pos = 0; pos < 8; pos++){
            currByte |= (unsigned char)(sdb[8 * (i + bOffset) + pos] * (1 << (7 - pos)));
        }
        *(data + i) = currByte;
    }
}

// Construct bit vector from the pointer to a byte array
sdsl::bit_vector constructBitVectorFromArray(const void* data, size_t dataSize){
    sdsl::bit_vector b(dataSize * 8ll);
    for(size_t i = 0; i < dataSize; i++){
        unsigned char currByte = *((unsigned char*)data + i);
        for(int pos = 0; pos < 8; pos++){
            b[8 * i + pos] = (bool)((currByte >> (7 - pos)) & 1);
        }
    }
    return b;
}

// Construct byte array from compressed bit vector
void* constructArrayFromSdVector(sd_vector<>& sdb){
    size_t dataSize = (sdb.size() + 7) / 8; // Size of original bit vector in bytes
    void* data = malloc_orDie(dataSize);
    copySdVectorSliceToArray((unsigned char*)data, sdb, 0ll, dataSize);
    return data;
}
 
#endif