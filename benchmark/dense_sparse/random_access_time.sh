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
    g++ -Wall -w -I/usr/local/include/ -c compression.cpp -lm
    g++ -L/usr/local/lib/ compression.o -o compressor -lzstd -lsdsl
    ./compressor $filePath $dictPath $chunkSize

    # # Decompressing
    g++ -Wall -w -I/usr/local/include/ -c decompression.cpp -lm
    g++ -L/usr/local/lib/ decompression.o -o decompressor -lzstd -lsdsl
    ./decompressor "$filePath"_ds.ds $dictPath
done
