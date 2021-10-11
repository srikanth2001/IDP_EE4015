# IDP_EE4015

## Create a dictionary
* Navigate to the folder containing your text files.
* Execute the following command.
``` $ zstd --train -B10240 --maxdict=102400 -o ../src/dict.zdict -r . ```

`--maxdict` flag is to specify the max size of the dictionary to be 100KB.
`-B` flag is to specify the chunk size of 10KB.
**Note:** You can either train the dictionary on a large number of small files, or you can divide a large file into small chunks.
* For more info, check https://manpages.debian.org/testing/zstd/zstd.1.en.html

## Compiling
* To compile dictionary_compression, run
    ```$ gcc -Wall -I/usr/local/include/ -c dictionary_compression.c -lm```
* For linking, use
    ```$ gcc -L/usr/local/lib/ dictionary_compression.o -o dict_compressor -lzstd```
* This will create an executable file called dict_compressor. To run this, use
    ```$ ./compressor raw_filename dict.zdict```

## Results

For a text file of size 100MB,
S. no. | Type of Compression  | Compressed size (in Bytes) | Compression ratio
| ------------- | ------------- | ------------- | ------------- |
| 1 | Simple Compression | 40,667,764 | 2.459 |
| 2 | Dictionary Compression | 35,631,311 | 2.806 |
| 3 | Dictionay Compression using chunks | 35,598,780 | 2.809 |

## Links
For the text files that were used, check https://drive.google.com/folderview?id=14VN-RFaqX92H5ZyzqTGSkkkEucMDDmQ1
