FILE_NAME="$1"

# Compression
noi=1
avgCompressionTime=0
compressedFileSize=0
avgDecompressionTime=0
for (( i=0; i<$noi; i+=1))
do
    start_time=$(date +%s.%3N)
    gzip -f "$FILE_NAME"
    end_time=$(date +%s.%3N)
    avgCompressionTime=$(echo "scale=3; $avgCompressionTime + $end_time - $start_time" | bc)
    compressedFileSize=$(stat -c%s "$FILE_NAME".gz)
    # avgCompressionTime=$(( $avgCompressiontime + $elapsed ))

    start_time=$(date +%s.%3N)
    gzip -fdk "$FILE_NAME".gz
    end_time=$(date +%s.%3N)
    # elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
    avgDecompressionTime=$(echo "scale=3; $avgDecompressionTime + $end_time - $start_time" | bc)
done

avgCompressionTime=$(echo "scale=3; $avgCompressionTime / $noi" | bc)
avgDecompressionTime=$(echo "scale=3; $avgDecompressionTime / $noi" | bc)
echo "Compressed file size: $(echo "scale=3; $compressedFileSize/(2^30)" | bc) GB"
echo "Compression time: $avgCompressionTime ms"
echo "Decompression time: $avgDecompressionTime ms"