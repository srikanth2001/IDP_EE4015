#!/bin/bash
filePath=$1
fileSize=$(stat -c%s "$filePath")
dictPath=$2
minSize=1024
maxSize=1048576
chunkSizes=(1024 5120 10240 102400 1048576)

for chunkSize in "${chunkSizes[@]}"
do
    echo -e "For chunk size = $chunkSize B"
    echo "Training dictionary...."
    zstd --train -B"$chunkSize" --maxdict=`expr $fileSize / 50` -o $dictPath -r $filePath
    echo ""
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c compression.cpp -lm
    g++ -L/usr/local/lib/ compression.o -o compressor -lzstd -lsdsl
    ./compressor $filePath $dictPath $chunkSize

    # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c decompression.cpp -lm
    g++ -L/usr/local/lib/ decompression.o -o decompressor -lzstd -lsdsl
    ./decompressor "$filePath"_bv_rlz.bv_rlz $dictPath
done
