#!/bin/bash
filePath=$1
minSize=1024
maxSize=1048576
chunkSizes=(1024 5120 10240 102400 1048576)

for chunkSize in "${chunkSizes[@]}"
do
    echo -e "For chunk size = $chunkSize B"
    echo ""
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c simple_compression.cpp -lm
    g++ -L/usr/local/lib/ simple_compression.o -o simple_compressor -lzstd -lsdsl
    ./simple_compressor $filePath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c simple_decompression.cpp -lm
    g++ -L/usr/local/lib/ simple_decompression.o -o simple_decompressor -lzstd -lsdsl
    ./simple_decompressor "$filePath".zst
done
