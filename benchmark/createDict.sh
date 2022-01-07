minSize=10240 # 10KB
maxSize=1048576 # 1MB
ds=`expr $maxSize - $minSize`
ds=`expr $ds / 10`
dictSize=$minSize
iter=1

while [[ $dictSize -le $maxSize ]]
do
    echo -e "Dictionary Size = $dictSize B"
    zstd --train -B10240 --maxdict=$dictSize -o ../data/dict_$iter.zdict -r ../data/enwik8
    echo ""
    dictSize=`expr $dictSize + $ds`
    iter=`expr $iter + 1`
done
