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
    g++ -Wall -w -I/usr/local/include/ -c dictionary_compression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_compression_chunks.o -o dict_compressor_chunks -lzstd -lsdsl
    ./dict_compressor_chunks $filePath $dictPath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_decompression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_decompression_chunks.o -o dict_decompressor_chunks -lzstd -lsdsl
    ./dict_decompressor_chunks "$filePath"_dict_chunks.zst $dictPath
done
