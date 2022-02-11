#!/bin/bash
filePath=$1
dictPath=$2
fileSize=$(stat -c%s "$filePath")
minNumOfChunks=$3
maxNumOfChunks=$4
minSize=`expr $fileSize / $maxNumOfChunks`
maxSize=`expr $fileSize / $minNumOfChunks`
ds=`expr \( $maxSize - $minSize + 9 \) / 10`

for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
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
    g++ -Wall -w -I/usr/local/include/ -c decompression_opt.cpp -lm
    g++ -L/usr/local/lib/ decompression_opt.o -o decompressor_opt -lzstd -lsdsl
    ./decompressor_opt "$filePath"_ds.ds $dictPath
done
