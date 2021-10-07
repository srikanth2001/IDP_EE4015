# IDP_EE4015

## Create a dictionary
* Navigate to the folder containing your data text files.
* ``` $zstd --train -B5242880 --maxdict=102400 -o ../src/dict.zdict -r . ```
Specify the max size of the dictionary to be 1MB.
For more info, check https://manpages.debian.org/testing/zstd/zstd.1.en.html