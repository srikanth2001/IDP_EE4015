import matplotlib.pyplot as plt
import seaborn as sns

rlzCompressedSize = [313334930, 312600501, 312439803, 312367375, 312335915, 312324174, 312311357, 312304335, 312290149, 312285861, 312282689]
rlzTime = [0.000724, 0.006790, 0.012810, 0.018691, 0.024614, 0.030549, 0.036578, 0.049006, 0.055119, 0.063100, 0.069792]
dsCompressedSize = []
dsTime = []

sns.set_theme()
plt.figure()
plt.plot(rlzCompressedSize, rlzTime, label="RLZ using chunking")
plt.plot(dsCompressedSize, dsTime, label="Tatwawadi-based")
plt.title("Compressed size v/s Random Access time")
plt.xlabel("Compressed file size (in B)")
plt.ylabel("Avg. random access time for a chunk (in s)")
plt.legend()
# plt.grid()
plt.savefig("../figs/random_access_times.png")
plt.savefig("../figs/random_access_times.svg")
plt.show()