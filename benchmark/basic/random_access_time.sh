filePath=$1
dictPath=$2
minSize=$3 
maxSize=$4
ds=`expr $maxSize - $minSize`
ds=`expr $ds / 10`

echo "Training dictionary...."
zstd --train -B`expr $maxSize / 500` --maxdict=`expr $maxSize / 5` -o $dictPath -r $filePath
echo ""

for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
do
    echo -e "For chunk size = $chunkSize B"
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_compression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_compression_chunks.o -o dict_compressor_chunks -lzstd
    ./dict_compressor_chunks $filePath $dictPath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_decompression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_decompression_chunks.o -o dict_decompressor_chunks -lzstd
    ./dict_decompressor_chunks "$filePath"_dict_chunks.zst  $dictPath
done
