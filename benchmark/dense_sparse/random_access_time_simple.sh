#!/bin/bash
filePath=$1
fileSize=$(stat -c%s "$filePath")
minNumOfChunks=$2
maxNumOfChunks=$3
minSize=`expr $fileSize / $maxNumOfChunks`
maxSize=`expr $fileSize / $minNumOfChunks`
ds=`expr \( $maxSize - $minSize + 9 \) / 10`

for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
do
    echo -e "For chunk size = $chunkSize B"
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c simple_compression.cpp -lm
    g++ -L/usr/local/lib/ simple_compression.o -o simple_compressor -lzstd -lsdsl
    ./simple_compressor $filePath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c simple_decompression.cpp -lm
    g++ -L/usr/local/lib/ simple_decompression.o -o simple_decompressor -lzstd -lsdsl
    ./simple_decompressor "$filePath".ds
done
