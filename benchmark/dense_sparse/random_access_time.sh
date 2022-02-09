#!/bin/bash
filePath=$1
dictPath=$2
fileSize=$(stat -c%s "$filePath")
minSize=`expr $fileSize / 10000`
maxSize=`expr $fileSize / 1000`
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

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c decompression.cpp -lm
    g++ -L/usr/local/lib/ decompression.o -o decompressor -lzstd -lsdsl
    ./decompressor "$filePath"_ds.ds $dictPath
done
