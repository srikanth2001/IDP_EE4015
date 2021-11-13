# IDP_EE4015

## Create a dictionary
* Navigate to the folder containing your text files.
* Execute the following command.
``` $ zstd --train -B10240 --maxdict=102400 -o [dictionary path] -r [data file path]```

`--maxdict` flag is to specify the max size of the dictionary to be 100KB.
`-B` flag is to specify the chunk size of 10KB.
**Note:** You can either train the dictionary on a large number of small files, or you can divide a large file into small chunks.
* For more info, check https://manpages.debian.org/testing/zstd/zstd.1.en.html

### Create dictionaries of multiple sizes from the same dataset
* Navigate to the `benchmark/` directory and add execute permissions to the `createDict.sh` script using-
``` $ chmod +x createDict.sh ```

* Run the script using -
    ``` $ ./createDict.sh ```

## Compiling
* To compile dictionary_compression, run
    ```$ gcc -Wall -I/usr/local/include/ -c dictionary_compression_chunks.c -lm```
* For linking, use
    ```$ gcc -L/usr/local/lib/ dictionary_compression_chunks.o -o dict_compressor_chunks -lzstd```
* This will create an executable file called dict_compressor. To run this, use
    ```$ ./dict_compressor_chunks raw_filename dict.zdict```

## Results

For a text file of size 100MB,
S. no. | Type of Compression  | Compressed size (in Bytes) | Compression ratio
| ------------- | ------------- | ------------- | ------------- |
| 1 | Simple Compression | 40,667,764 | 2.459 |
| 2 | Dictionary Compression | 35,631,311 | 2.806 |
| 3 | Dictionay Compression using chunks | 35,598,780 | 2.809 |

### Varying chunk size
The below data was obtained using dictionary compression by varying the chunk size from 500KB to 5MB.
S. no. | Chunk size (in Bytes)  | Compressed size (in Bytes) | Average compression time (in sec)
| ------------- | ------------- | ------------- | ------------- |
| 1 | 512000 | 36361961 | 0.714 |
| 2 | 985088  | 35793266 | 0.783 |
| 3 | 1458176 | 35739964 | 0.766 |
| 4 | 1931264 | 35704968 | 0.750 |
| 5 | 2404352 | 35685085 | 0.732 |
| 6 | 2877440 | 35664601 | 0.713 |
| 7 | 3823616 | 35656041 | 0.691 |
| 8 | 4296704 | 35642683 | 0.683 |
| 9 |  4769792 | 35648633 | 0.676 |
| 10 | 5242880 | 35631259 | 0.671 |

### Varying dictionary size
The below data was obtained using dictionary compression by varying the dictionary size from 10KB to 1MB.
S. no. | Dictionary size (in Bytes)  | Compressed size (in Bytes) | Average compression time (in sec)
| ------------- | ------------- | ------------- | ------------- |
| 1 | 10240 | 35690263 | 0.777 |
| 2 | 114073 | 35631514 | 0.672 |
| 3 | 217906 | 35611867 | 0.670 |
| 4 | 321739 | 35597781 |  0.665 |
| 5 | 425572 | 35566262 | 0.667 |
| 6 | 529405 | 35552081 |  0.667 |
| 7 | 633238  | 35536256 | 0.670 |
| 8 |  840904 | 35517253 | 0.673 |
| 9 | 944737 | 35564179 | 0.675 |
| 10 | 1048570 | 35491525 | 0.665 |

## Links
For the text files that were used, check https://drive.google.com/folderview?id=14VN-RFaqX92H5ZyzqTGSkkkEucMDDmQ1
