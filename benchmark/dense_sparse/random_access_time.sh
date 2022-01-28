minSize=512000 # 500KB
maxSize=5242880 # 5MB
ds=`expr $maxSize - $minSize`
ds=`expr $ds / 10`

# while [[ $dictSize -le $maxSize ]]
for (( chunkSize=$minSize; chunkSize<=$maxSize; chunkSize+=$ds ))
do
    echo -e "For chunk size = $chunkSize B"
    # Compressing
    g++ -Wall -w -I/usr/local/include/ -c compression.cpp -lm
    g++ -L/usr/local/lib/ compression.o -o compressor -lzstd -lsdsl
    ./compressor ../../data/enwik8 ../../data/dict.zdict $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c decompression.cpp -lm
    g++ -L/usr/local/lib/ decompression.o -o decompressor -lzstd -lsdsl
    ./decompressor ../../data/enwik8_ds.ds ../../data/dict.zdict
done
