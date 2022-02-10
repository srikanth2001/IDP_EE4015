#!/bin/bash
filePath=$1
dictPath=$2
fileSize=$(stat -c%s "$filePath")
minSize=$3
maxSize=$4
ds=`expr \( $maxSize - $minSize + 9 \) / 10`

for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
do
    echo -e "For chunk size = $chunkSize B"
    echo "Training dictionary...."
    zstd --train -B"$chunkSize" --maxdict=`expr $fileSize / 50` -o $dictPath -r $filePath
    echo ""
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_compression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_compression_chunks.o -o dict_compressor_chunks -lzstd
    ./dict_compressor_chunks $filePath $dictPath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_decompression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_decompression_chunks.o -o dict_decompressor_chunks -lzstd
    ./dict_decompressor_chunks "$filePath"_dict_chunks.zst $dictPath
done
