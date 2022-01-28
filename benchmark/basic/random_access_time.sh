minSize=512000 # 500KB
maxSize=5242880 # 5MB
ds=`expr $maxSize - $minSize`
ds=`expr $ds / 10`

# while [[ $dictSize -le $maxSize ]]
for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
do
    # echo -e "Dictionary Size = $dictSize B"
    # zstd --train -B10240 --maxdict=$dictSize -o ../data/dict_$iter.zdict -r ../data/enwik8
    # echo ""
    # dictSize=`expr $dictSize + $ds`
    # iter=`expr $iter + 1`
    echo -e "For chunk size = $chunkSize B"
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_compression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_compression_chunks.o -o dict_compressor_chunks -lzstd
    ./dict_compressor_chunks ../../data/enwik8 ../../data/dict.zdict $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c dictionary_decompression_chunks.cpp -lm
    g++ -L/usr/local/lib/ dictionary_decompression_chunks.o -o dict_decompressor_chunks -lzstd
    ./dict_decompressor_chunks ../../data/enwik8 ../../data/dict.zdict
done
